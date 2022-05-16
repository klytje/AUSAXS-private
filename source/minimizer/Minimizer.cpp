#include <minimizer/Minimizer.h>
#include <utility/Exceptions.h>

#include <functional>

using namespace mini;

Minimizer::Result::Result(const std::vector<double>& values, const std::vector<double>& errors, const std::vector<std::string>& names, double function_val) : fval(function_val) {
    if (values.size() != errors.size() || values.size() != names.size()) {throw except::size_error("Error in Minimizer::Result::Result: All provided vectors must be the same length.");}

    for (unsigned int i = 0; i < values.size(); i++) {
        this->parameters[names[i]] = values[i];
        this->errors[names[i]] = errors[i];
    }
}

Minimizer::Result::Result(const std::map<std::string, double>& params, const std::map<std::string, double>& errors, double function_val) : parameters(params), errors(errors), fval(function_val) {}

Minimizer::Minimizer(double(&f)(double*), unsigned int dim) {
    set_function(f, dim);
}

Minimizer::Minimizer(std::function<double(double*)> f, unsigned int dim) {
    set_function(f, dim);
}

void Minimizer::set_function(double(&f)(double*), unsigned int dim) {
    raw = std::bind(f, std::placeholders::_1);
    set_function(raw, dim);
}

void Minimizer::set_function(std::function<double(double*)> f, unsigned int dim) {
    if (dim == 0) {
        if (dimensionality == 0) {
            throw except::invalid_argument("Error in Minimizer::set_function: Dimension was not set at construction, and was not provided in this call.");
        }
    } else {
        dimensionality = dim;
    }
    
    raw = f;
    wrapper = [this] (double* par) {
        double fval = raw(par);
        std::vector<double> pars(par, par+dimensionality);
        evaluations.push_back(Evaluation(pars, fval));
        return fval;
    };

    function = wrapper;
}

void Minimizer::record_evaluations(bool setting) {
    function = setting ? wrapper : raw;
}