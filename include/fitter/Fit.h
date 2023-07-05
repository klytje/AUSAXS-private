#pragma once

#include <mini/detail/Landscape.h>
#include <mini/detail/Result.h>
#include <fitter/FitPlots.h>
#include <dataset/SimpleDataset.h>

#include <string>
#include <vector>

namespace fitter {
    class Fitter;

    class Fit : public mini::Result {
        public:
            Fit() noexcept {}

            /**
             * @brief Constructor.
             * 
             * Create a new Fit object based on a fitter and a minimizer result.
             */
            Fit(Fitter& fitter, const mini::Result& res, double chi2) noexcept;

            /**
             * @brief Constructor.
             * 
             * Create a new Fit object based on a minimizer result.
             */
            Fit(const mini::Result& res, double chi2, double dof) noexcept;
            
            /**
             * @brief Add the parameters from another fit to this one. Each parameter will count as an additional degree of freedom. 
             */ 
            void add_fit(Fitter& fit) noexcept;

            /**
             * @brief Add the parameters from another fit to this one. Each parameter will count as an additional degree of freedom. 
             */
            void add_fit(std::shared_ptr<Fit> fit) noexcept;

            /**
             * @brief Add plots to this fit.
             */
            void add_plots(Fitter& fitter);

            bool operator==(const Fit& other) const;

            /**
             * @brief Get a string representation of this object. 
             */
            [[nodiscard]] virtual std::string to_string() const noexcept;

            mini::Landscape evaluated_points;
            FitPlots figures;
            SimpleDataset residuals;
            unsigned int dof;
    };

    struct EMFit : public Fit {
        using Fit::Fit;

        [[nodiscard]] std::string to_string() const noexcept override;

        double level;
    };

    template<typename C>
    concept FitType = std::derived_from<C, Fit>;
}