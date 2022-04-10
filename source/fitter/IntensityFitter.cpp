#include <fitter/Fit.h>
#include <fitter/IntensityFitter.h>
#include <math/SimpleLeastSquares.h>
#include <ScatteringHistogram.h>
#include <Exceptions.h>

#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>

using std::string, std::vector, std::shared_ptr, std::unique_ptr;

shared_ptr<Fit> IntensityFitter::fit() {
    ROOT::Math::Minimizer* minimizer = ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS");
    auto f = std::bind(&IntensityFitter::chi2, this, std::placeholders::_1);
    ROOT::Math::Functor functor(f, 1); // declare the function to be minimized and its number of parameters
    minimizer->SetFunction(functor);
    minimizer->SetLimitedVariable(0, "c", 5, 1e-4, 0, 100); // scaling factor
    minimizer->Minimize();
    const double* res = minimizer->X();
    const double* err = minimizer->Errors();

    // apply c
    h.apply_water_scaling_factor(res[0]);
    vector<double> ym = h.calc_debye_scattering_intensity().get("I");
    vector<double> Im = splice(ym);

    // fit a, b
    SimpleLeastSquares fitter(Im, Io, sigma);
    std::shared_ptr<Fit> ab_fit = fitter.fit();

    fitted = std::make_shared<Fit>(fitter, minimizer, minimizer->MinValue());
    minimizer->SetPrintLevel(3);
    minimizer->PrintResults();
    return fitted;
}

vector<shared_ptr<TGraph>> IntensityFitter::plot() {
    if (fitted == nullptr) {throw except::bad_order("Error in IntensityFitter::plot: Cannot plot before a fit has been made!");}

    double a = fitted->params["a"];
    double b = fitted->params["b"];
    double c = fitted->params["c"];

    h.apply_water_scaling_factor(c);
    vector<double> ym = h.calc_debye_scattering_intensity().get("I");
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
    graphs[1] = std::make_shared<TGraph>(h.q.size(), &h.q[0], &ym_scaled[0]);
    graphs[2] = std::make_shared<TGraphErrors>(qo.size(), &qo[0], &Io[0], &xerr[0], &sigma[0]);
    return graphs;
}

unique_ptr<TGraphErrors> IntensityFitter::plot_residuals() {
    if (fitted == nullptr) {throw except::bad_order("Error in IntensityFitter::plot_residuals: Cannot plot before a fit has been made!");}
 
    double a = fitted->params["a"];
    double b = fitted->params["b"];
    double c = fitted->params["c"];

    h.apply_water_scaling_factor(c);
    vector<double> ym = h.calc_debye_scattering_intensity().get("I");
    vector<double> Im = splice(ym);

    // calculate the residuals
    vector<double> residuals(qo.size());
    for (size_t i = 0; i < qo.size(); ++i) {
        residuals[i] = ((Io[i] - a*Im[i]-b)/sigma[i]);
    }

    // prepare the TGraph
    vector<double> xerr(sigma.size(), 0);
    // unique_ptr<TGraphErrors> graph = std::make_unique<TGraphErrors>(qo.size(), &qo[0], &residuals[0], &xerr[0], &sigma[0]);
    unique_ptr<TGraphErrors> graph = std::make_unique<TGraphErrors>(qo.size(), &qo[0], &residuals[0], &xerr[0], &xerr[0]);
    return graph;
}

double IntensityFitter::chi2(const double* params) {
    double c = params[0];

    // apply c
    h.apply_water_scaling_factor(c);
    vector<double> ym = h.calc_debye_scattering_intensity().get("I");
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