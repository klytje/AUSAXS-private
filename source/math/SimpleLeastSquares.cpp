#include <vector>
#include <string>
#include <cmath>
#include <utility>
#include <memory>

#include <fitter/Fitter.h>
#include <math/SimpleLeastSquares.h>
#include <Exceptions.h>

#include <Math/SpecFuncMathCore.h> // for the incomplete gamma function

std::pair<double, double> SimpleLeastSquares::fit_params_only() {
    S = 0, Sx = 0, Sy = 0, Sxx = 0, Sxy = 0;
    for (size_t i = 0; i < x.size(); i++) {
        double sig2 = pow(y_err[i], 2);
        S += 1./sig2;
        Sx += x[i]/sig2;
        Sy += y[i]/sig2;
        Sxx += pow(x[i], 2)/sig2;
        Sxy += x[i]*y[i]/sig2;
    }

    delta = S*Sxx - pow(Sx, 2);
    a = (S*Sxy - Sx*Sy)/delta;
    b = (Sxx*Sy - Sx*Sxy)/delta;
    return std::make_pair(a, b);
}

std::shared_ptr<Fitter::Fit> SimpleLeastSquares::fit() {
    if (delta == 0) {fit_params_only();}
    double a_err2 = S/delta; // squared sigmas
    double b_err2 = Sxx/delta; 

    // double cov_ab = -Sx/delta;
    double Q = ROOT::Math::inc_gamma((double) x.size()/2 -1, chi2()/2);

    std::shared_ptr<Fit> f = std::make_shared<Fit>();
    f->params = {{"a", a}, {"b", b}};
    f->errors = {{"a", sqrt(a_err2)}, {"b", sqrt(b_err2)}};
    f->dof = x.size() - 2;
    f->chi2 = chi2();
    f->calls = 1;
    f->converged = Q > 0.001;
    return f;
}

double SimpleLeastSquares::chi2() const {
    double chi = 0;
    for (size_t i = 0; i < x.size(); ++i) {
        chi += pow((y[i] - a*x[i] - b)/y_err[i], 2);
    }
    return chi;
}

std::vector<std::shared_ptr<TGraph>> SimpleLeastSquares::plot() {
    throw except::unexpected("Error in SimpleLeastSquares::plot: Not implemented.");
}

std::unique_ptr<TGraphErrors> SimpleLeastSquares::plot_residuals() {
    throw except::unexpected("Error in SimpleLeastSquares::plot_residuals: Not implemented.");
}

unsigned int SimpleLeastSquares::dof() const {return x.size() - 2;}