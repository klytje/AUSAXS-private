#pragma once

#include <mini/Minimizer.h>

namespace mini {
	/**
	 * @brief A scanning minimizer. 
	 * 		  This algorithm performs a simple scan of the chi2 landscape, and then uses a local minimizer to find the minimum. 
	 * 		  This should only be used for relatively simple optimization problems. 
	 */
	class Scan : public Minimizer {
		public:
			Scan() = default;

			Scan(double(&func)(std::vector<double>), unsigned int evals = 100);

            Scan(std::function<double(std::vector<double>)> func, unsigned int evals = 100);

            Scan(double(&func)(std::vector<double>), const Parameter& param, unsigned int evals = 100);

            Scan(std::function<double(std::vector<double>)> func, const Parameter& param, unsigned int evals = 100);

            /**
             * @brief Destructor.
             */
            virtual ~Scan() override = default;

            /**
             * @brief Generate a landscape of the function.
             */
            Dataset2D landscape(unsigned int evals) override;

            /**
             * @brief Get the evaluated points and their function values.
             */
            Dataset2D get_evaluated_points() const override;

			/**
			 * @brief Set the number of evaluations. 
			 */
			void set_evals(unsigned int evals) noexcept;

			void add_parameter(const Parameter& param);

		private:
			unsigned int bins = 100;

			/**
			 * @brief Perform the minimization.
			 */
			Result minimize_override() override;

			void looper(std::vector<double>& p, unsigned int index) const;
	};
}