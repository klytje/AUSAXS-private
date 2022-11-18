#include <preprocessor.h>
#include <mini/Golden.h>
#include <utility/Exceptions.h>
#include <utility/Utility.h>

using namespace mini;

Golden::Golden(double(&func)(std::vector<double>)) : Minimizer(func) {}

Golden::Golden(std::function<double(std::vector<double>)> func) : Minimizer(func) {}

Golden::Golden(double(&func)(std::vector<double>), const Parameter& param) : Minimizer(func) {
    add_parameter(param);
}

Golden::Golden(std::function<double(std::vector<double>)> func, const Parameter& param) : Minimizer(func) {
    add_parameter(param);
}

Limit Golden::search(Limit bounds) const {
    // Code adapted from the python implementation from Wikipedia: https://en.wikipedia.org/wiki/Golden-section_search 
    double a = bounds.min, b = bounds.max;
    double temp = a + b;
    
    // sort such that a < b
    a = std::min(a, b);
    b = temp - a;

    double diff = b - a;
    if (__builtin_expect(diff < tol, false)) {
        return Limit(a, b);
    }

    // expected number of steps to reach tolerance
    unsigned int n = std::ceil(std::log(tol/diff)/std::log(invphi));

    double c = a + invphi2*diff;
    double d = a + invphi*diff;
    double fc = function({c});
    double fd = function({d});

    for (unsigned int k = 0; k < n-1; k++) {
        if (fc < fd) {
            b = d;
            d = c;
            fd = fc;
            diff = invphi*diff;
            c = a + invphi2*diff;
            fc = function({c});
        } else {
            a = c;
            c = d; 
            fc = fd;
            diff = invphi*diff;
            d = a + invphi*diff;
            d = a + invphi*diff;
            fd = function({d});
        }
    }

    if (fc < fd) {
        return Limit(a, d);
    } else {
        return Limit(c, b);
    }
}

Dataset2D Golden::landscape(unsigned int evals) {
    if (parameters.empty()) {throw except::bad_order("Golden::landscape: No parameters were supplied.");}
    std::vector<double> x, y;

    auto bounds = parameters[0].bounds.value();
    for (double val = bounds.min; val < bounds.max; val += bounds.span()/evals) {
        double fval = function({val});
        if (std::isnan(fval) || std::isinf(fval)) {
            debug_print("Warning in Golden::landscape: Function value is nan or inf and will be skipped.");
            continue;
        }
        x.push_back(val);
        y.push_back(fval);
    }

    return Dataset2D(x, y);
}

Dataset2D Golden::get_evaluated_points() const {
    if (evaluations.empty()) {throw except::bad_order("Golden::get_evaluated_points: Cannot get evaluated points before a minimization call has been made.");}

    unsigned int N = evaluations.size();
    std::vector<double> x(N), y(N);
    for (unsigned int i = 0; i < N; i++) {
        x[i] = evaluations[i].vals[0];
        y[i] = evaluations[i].fval;
    }
    return Dataset2D(x, y, "x", "f(x)");
}

Result Golden::minimize_override() {
    Limit optimal_interval = search(parameters[0].bounds.value());
    FittedParameter p(parameters[0], optimal_interval.center(), optimal_interval-optimal_interval.center());
    return Result(p, function({p.value}), fevals);
}

void Golden::add_parameter(const Parameter& param) {
    if (!param.has_bounds()) {throw except::invalid_argument("Golden::add_parameter: The parameter must be supplied with limits for this minimizer.");}
    if (!parameters.empty()) {throw except::invalid_operation("Golden::add_parameter: This minimizer only supports 1D problems.");}
    if (param.has_guess()) {debug_print("Warning in Golden::add_parameter: Guess value will be ignored.");}
    parameters.push_back(param);
}