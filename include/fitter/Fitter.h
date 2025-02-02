#pragma once

#include <fitter/FitterFwd.h>
#include <fitter/Fit.h>
#include <dataset/SimpleDataset.h>
#include <mini/Minimizer.h>

#include <vector>
#include <memory>

namespace fitter {
    class Fitter {
        public:
            virtual ~Fitter() {}

            /**
             * @brief Perform a fit and return a fit object containing various information. 
             */
            [[nodiscard]] virtual std::shared_ptr<Fit> fit() = 0;

            /**
             * @brief Perform a fit and return the minimum function value.
             */
            [[nodiscard]] virtual double fit_chi2_only() = 0;

            /**
             * @brief Make a plot of the fit. 
             * 
             * @return A vector of TGraphs {Interpolated points, Optimal line, Measured points with uncertainties}
             */
            [[nodiscard]] virtual FitPlots plot() = 0;

            /**
             * @brief Make a residual plot of the fit.
             * 
             * @return A TGraphErrors with the residuals and their uncertainties. 
             */
            [[nodiscard]] virtual SimpleDataset plot_residuals() = 0;

            [[nodiscard]] virtual std::shared_ptr<Fit> get_fit() const = 0;

            /**
             * @brief Get the number of degrees of freedom.
             */
            [[nodiscard]] virtual unsigned int dof() const = 0;

            /**
             * @brief Get the number of degrees of freedom.
             */
            [[nodiscard]] unsigned int degrees_of_freedom() const {return dof();}

            /**
             * @brief Get the total number of data points. 
             */
            [[nodiscard]] virtual unsigned int size() const = 0;

            /**
             * @brief Evaluate the chi2 function for the given parameters.
             */
            [[nodiscard]] virtual double chi2(const std::vector<double>& params) = 0;
    };
}