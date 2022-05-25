#include <minimizer/Utility.h>
#include <utility/Exceptions.h>

#include <algorithm>

using namespace mini;

Result::Result(const FittedParameter& param, double fval, unsigned int fevals) : parameters({param}), fval(fval), fevals(fevals) {}
Result::Result(const std::vector<FittedParameter>& params, double fval, unsigned int fevals) : parameters(params), fval(fval), fevals(fevals) {}
void Result::add_parameter(const FittedParameter& param) {parameters.push_back(param);}
size_t Result::size() const noexcept {return parameters.size();}
size_t Result::dim() const noexcept {return size();}

FittedParameter Result::get_parameter(std::string name) const {
    auto pos = std::find_if(parameters.begin(), parameters.end(), [&name] (const FittedParameter& param) {return param.name == name;});
    if (pos == parameters.end()) {throw except::unknown_argument("Error in Result::get_parameter: No parameter named \"" + name + "\" was found.");}
    return *pos;
}

FittedParameter Result::get_parameter(unsigned int index) const {
    if (size() < index) {throw except::out_of_bounds("Error in Result::get_parameter: Index \"" + std::to_string(index) + "\" is out of bounds (" + std::to_string(size()) + ").");}
    return parameters[index];
}


Evaluation::Evaluation(std::vector<double> vals, double fval) : vals(vals), fval(fval) {}


Parameter::Parameter(std::string name, Limit bounds) : name(name), bounds(bounds) {}
Parameter::Parameter(std::string name, double guess, Limit bounds) : name(name), guess(guess), bounds(bounds) {}
bool Parameter::has_bounds() const noexcept {return bounds.has_value();}
bool Parameter::has_guess() const noexcept {return guess.has_value();}
bool Parameter::has_name() const noexcept {return !name.empty();}
bool Parameter::empty() const noexcept {return !(has_name() && (has_bounds() || has_guess()));}
std::string Parameter::to_string() const noexcept {
    std::string s = name;
    if (has_guess()) {s += " guess " + std::to_string(guess.value());}
    if (has_bounds()) {s += " bounds [" + std::to_string(bounds.value().min) + std::to_string(bounds.value().max) + "]";}
    return s;
}


FittedParameter::FittedParameter(std::string name, double val, Limit error) : name(name), value(val), error(error) {}
FittedParameter::FittedParameter(std::string name, double val, double error) : name(name), value(val), error({-error, +error}) {}
FittedParameter::FittedParameter(const Parameter& param, double val, Limit error) : name(param.name), value(val), error(error) {}
FittedParameter::FittedParameter(const Parameter& param, double val, double error) : name(param.name), value(val), error(-error, +error) {}
std::string FittedParameter::to_string() const noexcept {
    return name + " " + std::to_string(value) + " " + error.to_string();
}
double FittedParameter::mean_error() const noexcept {
    return (value - error.min)*0.5 + (error.max - value)*0.5;
}

void FittedParameter::set_error(double error) noexcept {
    this->error = Limit(-error, error);
}

void FittedParameter::set_error(double min, double max) noexcept {
    this->error = Limit(min, max);
}