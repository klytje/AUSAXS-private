#include "fitter/Fitter.h"
#include "fitter/SimpleLeastSquares.h"
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

class IntensityFitter : public Fitter {
public: 
    /**
     * @brief Prepare a fit of the measured values in @a input to the model described by @a q and @a I.
     * @param input the path to the file containing the measured values. 
     * @param q the model q values.
     * @param I the model I values. 
     */
    // IntensityFitter(string input, vector<double>& q, vector<double>& I) : xm(q), ym(I) {setup(input, q, I);}
    // IntensityFitter(string input, ScatteringHistogram& h) : h(h) {setup(input);}
    IntensityFitter(string input, std::shared_ptr<ScatteringHistogram> h) : h(h), xm(h->q) {setup(input);}
    ~IntensityFitter() override {}

    /**
     * @brief Perform the fit.
     * @return A Fit object containing various information about the fit. Note that the fitted scaling parameter is a = c/M*r_e^2 and b = background
     */
    shared_ptr<Fit> fit() override {
        ROOT::Math::Minimizer* minimizer = ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS");
        auto f = std::bind(&IntensityFitter::chi2, this, std::placeholders::_1);
        ROOT::Math::Functor functor(f, 1); // declare the function to be minimized and its number of parameters
        minimizer->SetFunction(functor);
        minimizer->SetLimitedVariable(0, "c", 5, 1e-4, 0, 100); // scaling factor
        minimizer->Minimize();
        const double* res = minimizer->X();
        const double* err = minimizer->Errors();

        // apply c
        h->apply_water_scaling_factor(res[0]);
        vector<double> ym = h->calc_debye_scattering_intensity();
        vector<double> Im = splice(ym);

        // fit a, b
        SimpleLeastSquares fitter(Im, Io, sigma);
        std::shared_ptr<Fit> ab_fit = fitter.fit();
        
        bool converged = !minimizer->Status();
        std::map<string, double> pars = {{"c", res[0]}, {"a", ab_fit->params["a"]}, {"b", ab_fit->params["b"]}};
        std::map<string, double> errs = {{"c", res[0]}, {"a", ab_fit->errs["a"]}, {"b", ab_fit->errs["b"]}};
        double funcalls = minimizer->NCalls();
        fitted = std::make_shared<Fit>(pars, errs, chi2(res), qo.size()-2, funcalls, converged);
        minimizer->SetPrintLevel(3);
        minimizer->PrintResults();
        return fitted;
    }

    // shared_ptr<Fit> fit() override {
    //     SimpleLeastSquares fitter(Im_spliced, Io, sigma);
    //     fitted = fitter.fit();
    //     return fitted;
    // }

    vector<shared_ptr<TGraph>> plot() const {
        if (fitted == nullptr) {throw except::bad_order("Error in IntensityFitter::plot: Cannot plot before a fit has been made!");}

        double a = fitted->params["a"];
        double b = fitted->params["b"];
        double c = fitted->params["c"];

        h->apply_water_scaling_factor(c);
        vector<double> ym = h->calc_debye_scattering_intensity();
        vector<double> Im = splice(ym);

        // calculate the scaled I model values
        vector<double> I_scaled(qo.size()); // spliced data
        vector<double> ym_scaled(ym.size()); // original scaled data
        std::transform(Im.begin(), Im.end(), I_scaled.begin(), [&a, &b] (double I) {return I*a+b;});
        std::transform(ym.begin(), ym.end(), ym_scaled.begin(), [&a, &b] (double I) {return I*a+b;});

        // prepare the TGraphs
        vector<double> xerr(sigma.size(), 0);
        vector<shared_ptr<TGraph>> graphs(3);
        graphs[0] = std::make_shared<TGraph>(qo.size(), &qo[0], &I_scaled[0]);
        graphs[1] = std::make_shared<TGraph>(xm.size(), &xm[0], &ym_scaled[0]);
        graphs[2] = std::make_shared<TGraphErrors>(qo.size(), &qo[0], &Io[0], &xerr[0], &sigma[0]);
        return graphs;
    }

    unique_ptr<TGraphErrors> plot_residuals() {
        if (fitted == nullptr) {throw except::bad_order("Error in IntensityFitter::plot_residuals: Cannot plot before a fit has been made!");}

        double a = fitted->params["a"];
        double b = fitted->params["b"];
        double c = fitted->params["c"];

        h->apply_water_scaling_factor(c);
        vector<double> ym = h->calc_debye_scattering_intensity();
        vector<double> Im = splice(ym);

        // calculate the residuals
        vector<double> residuals(qo.size());
        for (size_t i = 0; i < qo.size(); ++i) {
            residuals[i] = ((Io[i] - a*Im[i]-b)/sigma[i]);
        }

        // prepare the TGraph
        vector<double> xerr(sigma.size(), 0);
        unique_ptr<TGraphErrors> graph = std::make_unique<TGraphErrors>(qo.size(), &qo[0], &residuals[0], &xerr[0], &sigma[0]);
        return graph;
    }

private: 
    shared_ptr<Fit> fitted;
    std::shared_ptr<ScatteringHistogram> h;
    vector<double> qo; // observed q values
    vector<double> Io; // observed I values
    vector<double> sigma; // error in Io

    const vector<double> &xm; // full x and y data from the model

    /**
     * @brief Calculate chi2 for a given choice of parameters @a params.
     */
    double chi2(const double* params) const {
        double c = params[0];

        // apply c
        h->apply_water_scaling_factor(c);
        vector<double> ym = h->calc_debye_scattering_intensity();
        vector<double> Im = splice(ym);

        // fit a, b
        SimpleLeastSquares fitter(Im, Io, sigma);
        auto[a, b] = fitter.fit_params_only();

        // calculate chi2
        double chi = 0;
        for (size_t i = 0; i < qo.size(); i++) {
            chi += pow((Io[i] - a*Im[i]-b)/sigma[i], 2);
        }
        return chi;
    }

    /**
     * @brief Prepare this class for fitting.
     * @param file measured values to compare the model against.
     */
    void setup(string file) {
        std::tie(qo, Io, sigma) = read(file); // read observed values from input file
    }

    /**
     * @brief Splice values from the model to fit the evaluation points defined by the q values of the input file. 
     * @param ym the model y-values corresponding to xm
     */
    vector<double> splice(const vector<double>& ym) const {
        vector<double> Im = vector<double>(qo.size()); // spliced model values
        CubicSpline s(xm, ym);
        for (size_t i = 0; i < qo.size(); ++i) {
            Im[i] = s.spline(qo[i]);
        }
        return Im;
    }

    std::tuple<vector<double>, vector<double>, vector<double>> read(string file) const {
        // check if file was succesfully opened
        std::ifstream input(file);
        if (!input.is_open()) {throw std::ios_base::failure("Error in IntensityFitter::read: Could not open file \"" + file + "\"");}

        vector<double> q;
        vector<double> I;
        vector<double> sigma;
        string line; // placeholder for the current line
        while(getline(input, line)) {
            if (line[0] == ' ') {line = line.substr(1);} // fix leading space
            vector<string> tokens;
            boost::split(tokens, line, boost::is_any_of(" ,\t")); // spaces, commas, and tabs can all be used as separators (but not a mix of them)

            // determine if we are in some sort of header
            if (tokens.size() < 3 || tokens.size() > 4) {continue;} // too many separators
            bool skip = false;
            for (int i = 0; i < 3; i++) { // check if they are numbers
                if (!tokens[i].empty() && tokens[i].find_first_not_of("0123456789-.Ee") != string::npos) {skip = true;}
            }
            if (skip) {continue;}

            // now we are most likely beyond any headers
            double _q, _I, _sigma;
            _q = std::stod(tokens[0]); // we know for sure that the strings are convertible to numbers (boost check)
            _I = std::stod(tokens[1]);
            _sigma = std::stod(tokens[2]);

            if (_q > 10) {continue;} // probably not a q-value if it's larger than 10

            // check user-defined limits
            if (_q < setting::fit::q_low) {continue;}
            if (_q > setting::fit::q_high) {continue;}

            // add the values to our vectors
            q.push_back(_q);
            I.push_back(_I);
            sigma.push_back(_sigma); 
        }
        return std::make_tuple(q, I, sigma);
    }
};