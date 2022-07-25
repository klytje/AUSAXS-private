#pragma once

#include <math/Matrix.h>
#include <utility/Exceptions.h>
#include <utility/PointSet.h>

/**
 * @brief A representation of a dataset. The set consists of fixed number of named columns, with a variable number of rows. 
 */
class Dataset : public Matrix<double> {
    public: 
        /**
         * @brief Default constructor. 
         */
        Dataset() {}

        /**
         * @brief Create a new dataset with the given columns.
         */
        Dataset(std::vector<std::string> col_names) : Matrix(0, col_names.size()), names(col_names) {}

        /**
         * @brief Create a new dataset with the given columns.
         */
        Dataset(std::vector<std::vector<double>> cols, std::vector<std::string> col_names) : Matrix(cols), names(col_names) {}

        /**
         * @brief Create a new dataset with the given columns.
         */
        Dataset(std::vector<std::vector<double>> cols);

        /**
         * @brief Create a new dataset with the specified dimensions. 
         */
        Dataset(unsigned int rows, unsigned int cols);

        /**
         * @brief Destructor.
         */
        virtual ~Dataset() = default;

        /**
         * @brief Get a column based on its name. 
         */
        [[nodiscard]] Column<double> col(std::string column);

        /**
         * @brief Get a column based on its name. 
         */
        [[nodiscard]] const ConstColumn<double> col(std::string column) const;

        /**
         * @brief Get a column based on its index.
         */
        [[nodiscard]] Column<double> col(unsigned int index);

        /**
         * @brief Get a column based on its index.
         */
        [[nodiscard]] const ConstColumn<double> col(unsigned int index) const;

        /**
         * @brief Get a row based on its index.
         */
        [[nodiscard]] Row<double> row(unsigned int index);

        /**
         * @brief Get a row based on its index.
         */
        [[nodiscard]] const ConstRow<double> row(unsigned int index) const;

        /**
         * @brief Get the number of points in the dataset.
         */
        [[nodiscard]] size_t size() const noexcept;

        /**
         * @brief Assign a Matrix to this dataset.
         */
        void operator=(const Matrix<double>&& other);

        /**
         * @brief Write this dataset to the specified file. 
         */
        void save(std::string path) const;

        /**
         * @brief Load a dataset from the specified file. 
         */
        virtual void load(std::string path);

        /**
         * @brief Set the column names. 
         */
        void set_col_names(std::vector<std::string> names);

        /**
         * @brief Set a column name. 
         */
        void set_col_names(unsigned int i, std::string name);

        /**
         * @brief Get the column names. 
         */
        [[nodiscard]] std::vector<std::string> get_col_names();

        /**
         * @brief Get a column name. 
         */
        [[nodiscard]] std::string get_col_names(unsigned int i);

    private: 
        std::vector<std::string> names; // The column names
};