#pragma once

#include <container/ArrayContainer2D.h>
#include <constants/Constants.h>
#include <table/DebyeTable.h>

#include <cmath>
#include <vector>

namespace table {
    /**
     * @brief sinc(x) lookup table for the Debye transform.
     * 
     * This table is evaluated at compile-time for the default q and d axes defined in the constants namespace.
     */
    class ArrayDebyeTable : public DebyeTable, private container::ArrayContainer2D<constants::axes::d_type, constants::axes::q_axis.bins, constants::axes::d_axis.bins> {
        public:
            /**
             * @brief Initialize a compile-time sinc lookup table.
             * 
             * Only a single instance of this class is allowed, and it is initialized at compile-time.
             */
            constexpr ArrayDebyeTable() noexcept {
                initialize();
            }

            // note: constexpr destructor cannot be defaulted due to GCC bug 93413
            constexpr ~ArrayDebyeTable() noexcept override {}

            /**
             * @brief Look up a value in the table based on indices. This is a constant-time operation. 
             */
            [[nodiscard]] constexpr double lookup(unsigned int q_index, unsigned int d_index) const override {return index(q_index, d_index);}

            /**
             * @brief Get the size of the table in the q-direction. 
             */
            [[nodiscard]] constexpr unsigned int size_q() const noexcept override {return ArrayContainer2D::N;}

            /**
             * @brief Get the size of the table in the d-direction. 
             */
            [[nodiscard]] constexpr unsigned int size_d() const noexcept override {return ArrayContainer2D::M;}

            /**
             * @brief Get an iterator to the beginning of the d-values for the given q-index.
             */
            [[nodiscard]] std::array<constants::axes::d_type, constants::axes::d_axis.bins>::const_iterator begin(unsigned int q_index) const override {return ArrayContainer2D::begin(q_index);}

            /**
             * @brief Get an iterator to the end of the d-values for the given q-index.
             */
            [[nodiscard]] std::array<constants::axes::d_type, constants::axes::d_axis.bins>::const_iterator end(unsigned int q_index) const override {return ArrayContainer2D::end(q_index);}

            /**
             * @brief Get the default table. 
             */
            [[nodiscard]] static const ArrayDebyeTable& get_default_table();

            /**
             * @brief Check if the two vectors are compatible with the default table. 
             *        Note that this check is only performed in debug mode.
             */
            static void check_default(const std::vector<double>& q, const std::vector<constants::axes::d_type>& d);

            /**
             * @brief Check if the vector is compatible with the default table. 
             *        Note that this check is only performed in debug mode.
             */
            static void check_default(const std::vector<constants::axes::d_type>& d);

        private: 
            constexpr void initialize() noexcept {
                double tolerance = 1e-3;  // The minimum x-value where sin(x)/x is replaced by its Taylor-series.
                double inv_6 = 1./6;      // 1/6
                double inv_120 = 1./120;  // 1/120

                for (unsigned int i = 0; i < size_q(); ++i) {
                    double q = constants::axes::q_vals[i];
                    for (unsigned int j = 0; j < size_d(); ++j) {
                        constants::axes::d_type d = constants::axes::d_vals[j];
                        double qd = q*d;
                        if (qd < tolerance) {
                            double qd2 = qd*qd;
                            index(i, j) = 1 - qd2*inv_6 + qd2*qd2*inv_120;
                        } else {
                            index(i, j) = std::sin(qd)/qd;
                        }
                    }
                }
            }
    };
}