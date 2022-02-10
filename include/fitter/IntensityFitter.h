#pragma once

#include "fitter/SimpleIntensityFitter.h"
#include "math/CubicSpline.h"
#include "settings.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <tuple>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include "ScatteringHistogram.h"
#include "Exceptions.h"

#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>
#include <TGraph.h>
#include <TGraphErrors.h>

using std::string, std::vector, std::shared_ptr, std::unique_ptr;

class IntensityFitter : public SimpleIntensityFitter {
  public: 
    /**
     * @brief Constructor.
     *        Prepare a fit of the measured values in @a input to the model described by @a q and @a I.
     * 
     * @param input the path to the file containing the measured values. 
     * @param q the model q values.
     * @param I the model I values. 
     */
    IntensityFitter(string input, const ScatteringHistogram& h) : SimpleIntensityFitter(input, h) {}

    /**
     * @brief Constructor.
     *        Prepare a fit of the measured values in @a input to the model described by @a q and @a I.
     * 
     * @param input the path to the file containing the measured values. 
     * @param q the model q values.
     * @param I the model I values. 
     */
    IntensityFitter(string input, ScatteringHistogram&& h) : SimpleIntensityFitter(input, h) {}

    /**
     * @brief Destructor.
     */
    ~IntensityFitter() override = default;

    /**
     * @brief Perform the fit.
     * 
     * @return A Fit object containing various information about the fit. Note that the fitted scaling parameter is a = c/M*r_e^2 and b = background
     */
    shared_ptr<Fit> fit() override;

    /**
     * @brief Make a plot of the fit. 
     * 
     * @return A vector of TGraphs {Interpolated points, Optimal line, Measured points with uncertainties}
     */
    vector<shared_ptr<TGraph>> plot() override;

    /**
     * @brief Make a residual plot of the fit.
     * 
     * @return A TGraphErrors with the residuals and their uncertainties. 
     */
    unique_ptr<TGraphErrors> plot_residuals() override;

  private: 
    /**
     * @brief Calculate chi2 for a given choice of parameters @a params.
     */
    double chi2(const double* params) override;
};