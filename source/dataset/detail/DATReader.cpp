/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <dataset/detail/DATReader.h>
#include <dataset/Dataset.h>
#include <dataset/SimpleDataset.h>
#include <dataset/Dataset2D.h>
#include <utility/StringUtils.h>
#include <utility/Console.h>
#include <utility/Exceptions.h>
#include <math/Statistics.h>
#include <settings/HistogramSettings.h>
#include <settings/GeneralSettings.h>

#include <vector>
#include <string>
#include <fstream>

std::unique_ptr<Dataset> detail::DATReader::construct(const io::ExistingFile& path, unsigned int expected_cols) {
    if (settings::general::verbose) {
        console::print_info("\nReading dataset from \"" + path + "\"");
    }

    // check if file was succesfully opened
    std::ifstream input(path);
    if (!input.is_open()) {throw std::ios_base::failure("DATReader::construct: Could not open file \"" + path + "\"");}

    std::string line;
    std::vector<std::string> header;
    std::vector<std::vector<double>> row_data;
    std::vector<unsigned int> col_number;
    while(getline(input, line)) {
        // skip empty lines
        if (line.empty()) {continue;}

        // remove leading whitespace
        std::vector<std::string> tokens = utility::split(line, " ,\t\n\r"); // spaces, commas, and tabs can be used as separators

        // remove empty tokens
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (tokens[i].empty()) {
                tokens.erase(tokens.begin() + i);
                i--;
            }
        }

        // check if all tokens are numbers
        bool skip = false;
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (tokens[i].find_first_not_of("0123456789+-.Ee\n\r") != std::string::npos) {
                skip = true;
            }
        }
        if (skip) {
            header.push_back(line);
            continue;
        }

        // add values to dataset
        std::vector<double> vals(tokens.size());
        for (unsigned int i = 0; i < tokens.size(); i++) {
            vals[i] = std::stod(tokens[i]);
        }
        row_data.push_back(vals);
        col_number.push_back(vals.size());
    }

    // determine the most common number of columns, since that will likely be the data
    unsigned int mode = stats::mode(col_number);
    if (settings::general::verbose) {
        switch (mode) {
            case 2: 
                std::cout << "\t2 columns detected. Assuming the format is [q | I]" << std::endl;
                break;
            case 3:
                std::cout << "\t3 columns detected. Assuming the format is [q | I | Ierr]" << std::endl;
                break;
            case 4:
                std::cout << "\t4 columns detected. Assuming the format is [q | I | Ierr | qerr]" << std::endl;
                break;
            default:
                std::cout << "\t" << mode << " columns detected. Assuming the format is [q | I | Ierr | qerr | ...]" << std::endl;
        }
    }

    // check that we have at least the expected number of columns
    if (expected_cols != 0 && mode < expected_cols) {
        throw except::io_error("DATReader::construct: File has too few columns. Expected" + std::to_string(expected_cols) + " but found " + std::to_string(mode) + ".");
    }

    // copy the data to the dataset
    std::unique_ptr<Dataset> dataset;
    unsigned int count = 0;
    {
        // first copy all rows with the most common number of columns to a temporary vector
        std::vector<std::vector<double>> data_cols;
        for (unsigned int i = 0; i < row_data.size(); i++) {
            if (row_data[i].size() != mode) {continue;}     // skip rows with the wrong number of columns
            if (count++ < settings::axes::skip) {continue;}  // skip the first few rows if requested
            data_cols.push_back(std::move(row_data[i]));
        }

        // having too many columns is not a problem, but we should inform the user and then ignore the extra columns
        if (expected_cols != 0 && mode != expected_cols) {
            // shorten the data to the expected number of columns
            for (unsigned int i = 0; i < data_cols.size(); i++) {
                std::vector<double> row(expected_cols);
                for (unsigned int j = 0; j < expected_cols; j++) {
                    row[j] = data_cols[i][j];
                }
                data_cols[i] = std::move(row);
            }
            mode = expected_cols; // update mode to the number of columns we actually have
        }

        // add the data to the dataset
        // dataset = std::make_shared<Dataset>(std::move(data_cols));
        dataset = std::make_unique<Dataset>(0, mode);
        for (unsigned int i = 0; i < data_cols.size(); i++) {
            dataset->push_back(data_cols[i]);
        }
    }
    std::vector<std::string> col_names = {"q", "I", "Ierr", "qerr"};
    col_names.resize(mode);
    dataset->set_col_names(col_names);

    // skip the first few rows if requested
    if (settings::axes::skip != 0 && settings::general::verbose) {
        std::cout << "\tSkipped " << count - dataset->size_rows() << " data points from beginning of file." << std::endl;
    }

    // verify that at least one row was read correctly
    if (dataset->empty()) {
        throw except::io_error("DATReader::construct: No data could be read from the file.");
    }

    // scan the headers for units. must be either [Å] or [nm]
    bool found_unit = false;
    for (auto& s : header) {
        if (s.find("[nm]") != std::string::npos) {
            if (settings::general::verbose) {std::cout << "\tUnit [nm] detected. Scaling all q values by 1/10." << std::endl;}
            for (unsigned int i = 0; i < dataset->size_rows(); i++) {
                dataset->index(i, 0) /= 10;
            }
            found_unit = true;
            break;
        } else if ((s.find("[Å]") != std::string::npos) || (s.find("[AA]") != std::string::npos)) {
            if (settings::general::verbose) {std::cout << "\tUnit [Å] detected. No scaling necessary." << std::endl;}
            found_unit = true;
            break;
        }
    }
    if (!found_unit) {
        if (settings::general::verbose) {std::cout << "\tNo unit detected. Assuming [Å]." << std::endl;}
    }

    // remove all rows outside the specified q-range
    unsigned int N = dataset->size_rows();
    dataset->limit_x(settings::axes::qmin, settings::axes::qmax);
    if (N != dataset->size_rows() && settings::general::verbose) {
        std::cout << "\tRemoved " << N - dataset->size_rows() << " data points outside specified q-range [" << settings::axes::qmin << ", " << settings::axes::qmax << "]." << std::endl;
    }

    // check if the file is abnormally large
    if (dataset->size_rows() > 300) {
        // reread first line
        input.clear();
        input.seekg(0, input.beg);
        getline(input, line);

        // check if file has already been rebinned
        if (line.find("REBINNED") == std::string::npos) {
            // if not, suggest it to the user
            if (settings::general::verbose) {
                std::cout << "\tFile contains more than 300 rows. Consider rebinning the data." << std::endl;
            }
        }
    }

    if (settings::general::verbose) {
        std::cout << "\tSuccessfully read " << dataset->size_rows() << " data points from " << path << std::endl;
    }
    return dataset;
}