#include <fitter/Fit.h>
#include <iomanip>
#include <sstream>

Fit::Fit(Fitter& fitter, const ROOT::Math::Minimizer* const minimizer, double chi2) : chi2(chi2) {
    unsigned int vars = minimizer->NDim();
    const double* result = minimizer->X();
    const double* errs = minimizer->Errors();
    for (unsigned int i = 0; i < vars; i++) {
        params.insert({minimizer->VariableName(i), result[i]});
        errors.insert({minimizer->VariableName(i), errs[i]});
    }

    normal_plot = fitter.plot();
    residual_plot = fitter.plot_residuals();

    converged = minimizer->Status() == 0;
    calls = minimizer->NCalls();
    dof = fitter.dof() - vars;
}

Fit::Fit(std::map<std::string, double>& params, std::map<std::string, double>& errs, const double chi2, const int dof, const int calls, const bool converged) : 
    params(params), errors(errs), chi2(chi2), dof(dof), calls(calls), converged(converged) {}

template<typename T>
struct print_element {
    print_element(T t, int width) : t(t), width(width) {}

    friend std::ostream& operator<<(std::ostream& os, const print_element<T> e) {os << std::left << std::setw(e.width) << std::setprecision(e.width) << e.t; return os;}

    T t;
    int width;
};

std::string Fit::to_string() const {
    std::stringstream ss;
    ss << "\n+----------------------------------------------------------+"
       << "\n|                       FIT REPORT                         |"
       << "\n+----------------------------------------------------------+"
       << "\n| Converged: " << (converged ? "yes" : "no ") << "                               Fevals: " << print_element(calls, 4) << " |"
       << "\n| chi2: " << print_element(chi2, 10) << " dof: " << print_element(dof, 6) << " chi2/dof: " << print_element(chi2/dof, 10) << "   |"
       << "\n+----------------------------------------------------------+"
       << "\n| PAR |     VAL     |     UNC     |                        |";
    for (const auto& e : params) {
        ss << "| " << print_element(e.first, 4) << " | " << print_element(e.second, 10) << " | " << print_element(errors.at(e.first), 10)  << "                       |\n";
    }
    ss << "+----------------------------------------------------------+\n";

    return ss.str();
}