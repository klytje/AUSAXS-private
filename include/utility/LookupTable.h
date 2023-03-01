#pragma once

#include <utility/Table.h>

namespace table {
    /**
     * @brief A generic representation of a lookup table. The class can be templated to use any data type as indices. 
     */
    template<typename T, typename Q>
    class LookupTable : public Table {
        public: 
            /**
             * @brief Default constructor.
             */
            LookupTable() {}

            /**
             * @brief Constructor.
             * 
             * @param rows The row vector. Each element of the vector can later be used as an index. 
             * @param columns The column vector. Each element of the vector can later be used as an index. 
             */
            LookupTable(const std::vector<T>& rows, const std::vector<Q>& columns) {
                initialize(rows, columns);
            }

            /**
             * @brief Initialize the table with new index vectors. 
             * 
             * @param rows The row vector. Each element of the vector can later be used as an index. 
             * @param columns The column vector. Each element of the vector can later be used as an index. 
             */
            void initialize(const std::vector<T>& rows, const std::vector<Q>& columns) {
                N = rows.size(); M = columns.size();
                data.resize(N*M);

                Tmap.reserve(N);
                for (unsigned int i = 0; i < N; i++) {
                    Tmap.emplace(rows[i], i);
                    // Tmap[rows[i]] = i;
                }

                Qmap.reserve(M);
                for (unsigned int i = 0; i < M; i++) {
                    Qmap.emplace(columns[i], i);
                    // Qmap[columns[i]] = i;
                }
            }

            /**
             * @brief Check if the table has been initialized. 
             */
            bool is_empty() const {return data.empty();}

            /**
             * @brief Lookup a given integer index in the table. 
             *        Equivalent to index(i, j).
             * 
             * @param i The integer row index.
             * @param j The integer column index.
             */
            double lookup_index(int i, int j) const {return index(i, j);}

            /**
             * @brief Lookup a given index in the table. 
             * 
             * @param row The row element index.
             * @param col The column element index.
             */
            double lookup(const T row, const Q col) const {return index(Tmap.at(row), Qmap.at(col));}

            /**
             * @brief Assign to a given integer index in the table. 
             *        Equivalent to index(i, j).
             * 
             * @param i The integer row index.
             * @param j The integer column index.
             * @param val The value to store at this location. 
             */
            void assign_index(int i, int j, double val) {index(i, j) = val;}

            /**
             * @brief Assign to a given index in the table. 
             * 
             * @param row The row element index.
             * @param col The column element index.
             * @param val The value to store at this location. 
             */
            void assign(const T row, const Q col, double val) {index(Tmap.at(row), Qmap.at(col)) = val;}

        private:
            std::unordered_map<T, unsigned int> Tmap; // A map from row element indices to actual indices.
            std::unordered_map<Q, unsigned int> Qmap; // A map from column element indices to actual indices. 
    };    
}