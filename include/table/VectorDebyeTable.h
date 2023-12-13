#pragma once

#include <table/Table.h>
#include <table/DebyeTable.h>
#include <utility/Concepts.h>

namespace table {
    class VectorDebyeTable : public DebyeTable, private Table {
        public:
            VectorDebyeTable();

            /**
             * @brief Initialize a new runtime sinc lookup table for the given d-axis. 
             *        The default q-axis from costants::axes::q_axis will be used.
             */
            VectorDebyeTable(const std::vector<constants::axes::d_type>& d);

            /**
             * @brief Initialize a new runtime sinc lookup table for the given d-axis. 
             *        The default q-axis from costants::axes::q_axis will be used.
             */
            VectorDebyeTable(const std::array<constants::axes::d_type, constants::axes::d_axis.bins>& d);

            /**
             * @brief Look up a value in the table based on indices. This is a constant-time operation. 
             */
            [[nodiscard]] double lookup(unsigned int q_index, unsigned int d_index) const override;

            /**
             * @brief Get the size of the table in the q-direction. 
             */
            [[nodiscard]] std::size_t size_q() const noexcept override;

            /**
             * @brief Get the size of the table in the d-direction. 
             */
            [[nodiscard]] std::size_t size_d() const noexcept override;

            /**
             * @brief Get an iterator to the beginning of the d-values for the given q-index.
             */
            [[nodiscard]] const constants::axes::d_type* begin(unsigned int q_index) const override;

            /**
             * @brief Get an iterator to the end of the d-values for the given q-index.
             */
            [[nodiscard]] const constants::axes::d_type* end(unsigned int q_index) const override;

            /**
             * @brief Get an iterator to the beginning of the d-values for the given q-index.
             */
            [[nodiscard]] constants::axes::d_type*  begin(unsigned int q_index);

            /**
             * @brief Get an iterator to the end of the d-values for the given q-index.
             */
            [[nodiscard]] constants::axes::d_type*  end(unsigned int q_index);

        private: 
            /**
             * @brief Initialize this class for the given input. 
             * 
             * @param q The scattering vector to generate lookup values for. 
             * @param d The distance histogram to generate lookup values for. 
             */
            template<container_type T1, container_type T2>
            void initialize(const T1& q, const T2& d);

            /**
             * @brief Check if a DebyeTable is empty.
             */
            bool is_empty() const;
    };
}