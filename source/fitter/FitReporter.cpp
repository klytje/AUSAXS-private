#include <fitter/FitReporter.h>
#include <Exceptions.h>
#include <string>
#include <fstream>

void FitReporter::report(const Fit& fit) {
    std::cout << fit.to_string() << std::endl;
}

void FitReporter::report(const std::vector<Fit>& fits, std::vector<std::string> titles) {
    if (!titles.empty() && titles.size() != fits.size()) {throw except::size_error("Error in FitReporter::report: Size of fits and titles must be equal.");}

    auto title_reporter = get_title_reporter(titles);
    for (unsigned int i = 0; i < fits.size(); i++) {
        std::cout << title_reporter(titles[i]);
        std::cout << fits[i].to_string() << std::endl;
    }
}

void FitReporter::save(std::string path, const Fit& fit) {
    std::ofstream out(path);
    if (!out.is_open()) {throw except::io_error("Error in FitReporter::save: Could not open file path \"" + path + "\".");}
    out << fit.to_string() << std::endl;
    out.close();
}

void FitReporter::save(std::string path, const std::vector<Fit>& fits, std::vector<std::string> titles) {
    if (!titles.empty() && titles.size() != fits.size()) {throw except::size_error("Error in FitReporter::report: Size of fits and titles must be equal.");}

    std::ofstream out(path);
    if (!out.is_open()) {throw except::io_error("Error in FitReporter::save: Could not open file path \"" + path + "\".");}

    auto title_reporter = get_title_reporter(titles);
    for (unsigned int i = 0; i < fits.size(); i++) {
        out << title_reporter(titles[i]);
        out << fits[i].to_string() << std::endl;
    }
    out.close();
}

std::function<std::string(std::string)> FitReporter::get_title_reporter(std::vector<std::string> titles) {
    std::function<std::string(std::string)> title_reporter;
    if (titles.empty()) {
        title_reporter = [] (std::string) {return "";};
    } else {
        title_reporter = [] (std::string title) {
            string output;
            output += "\n+----------------------------------------------------------+";

            int spaces = (60 - title.size())/2 - 1;
            string spacing(spaces, ' ');
            output += "\n|" + spacing + title + (title.size() % 2 == 0 ? spacing : spacing + " ") + "|\n";
            return output;
        };
    }

    return title_reporter;
}