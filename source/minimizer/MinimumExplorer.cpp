#include <minimizer/MinimumExplorer.h>
#include <utility/Exceptions.h>
#include <utility/Utility.h>

using namespace mini;

MinimumExplorer::MinimumExplorer(double(&func)(const double*), unsigned int evals) : Minimizer(func), evals(evals) {}

MinimumExplorer::MinimumExplorer(std::function<double(const double*)> func, unsigned int evals) : Minimizer(func), evals(evals) {}

MinimumExplorer::MinimumExplorer(double(&func)(const double*), const Parameter& param, unsigned int evals) : Minimizer(func), evals(evals) {
    add_parameter(param);
}

MinimumExplorer::MinimumExplorer(std::function<double(const double*)> func, const Parameter& param, unsigned int evals) : Minimizer(func), evals(evals) {
    add_parameter(param);
}

//! Not currently overriding super method! Fix inheritance...
Dataset2D MinimumExplorer::landscape(unsigned int evals) {
    if (parameters.empty()) {throw except::bad_order("Error in MinimumExplorer::landscape: No parameters were supplied.");}

    const Parameter& param = parameters[0];
    double xmin = *param.guess;
    double fmin = function(&xmin);
    const double xmid = xmin;
    double x = xmin;

    // determine spacing required for change in function value
    double spacing = 1e-5;
    if (param.has_bounds()) {
        spacing = std::min(spacing, (param.bounds->max - param.bounds->min)/evals);
    }

    record_evaluations(false);  // disable recording while determining spacing
    unsigned int vchanges = 0;  // we want at least 2 changes for a decent estimate
    unsigned int counter = 0;   // counter to prevent infinite loop
    double factor = 2;          // step scaling factor
    while (counter < 50) {
        x += spacing;
        double f = function(&x);

        // check if the function value changed
        if (1e-6 < std::abs(f - fmin)) {
            vchanges++;
            factor = 1.3;                   // scale slower since we're close to the final step size
            if (2 < vchanges) {break;}      // stop after fval changed twice
        } else {
            spacing *= factor;
        }

        if (50 < counter++) {break;}
    }
    if (counter == 50) {throw except::bad_order("Error in MinimumExplorer::landscape: Could not determine spacing for landscape.");}
    spacing /= 2; // step size is twice the distance between fval changes after ending the earlier loop

    // get an estimate of the minimum value
    record_evaluations(true); // start recording again

    // go three steps to the left
    x = xmid;               // go back to the middle
    double fprev = fmin;    // keep track of last value
    counter = 0;            // reset counter
    for (int i = 0; i < 4; i++) {
        x -= spacing;
        double f = function(&x);

        // check if the function value actually changed
        if (std::abs(fprev - f) < 1e-6) {
            // if not, refine the spacing and try again
            x += spacing;
            spacing *= 1.3;
            i--;
            evaluations.pop_back();
        }

        // check if this is a new minimum
        if (f < fmin) {
            fmin = f;
            xmin = x;
            continue;
        } 

        // check if this value is higher than the previous one
        if (fprev < f) {
            fprev = f;
            counter++;
        }
    }
    // if counter == 4 the function is monotonically increasing to the left, and we shouldn't explore it further
    bool left = !(counter == 4);

    // go three steps to the right
    x = xmid;       // go back to the middle
    fprev = fmin;   // reset fprev
    counter = 0;    // reset counter
    for (int i = 0; i < 4; i++) {
        x += spacing;
        double f = function(&x);

        // check if the function value actually changed
        if (std::abs(fprev - f) < 1e-6) {
            // if not, refine the spacing and try again
            x -= spacing;
            spacing *= 1.3;
            i--;
            evaluations.pop_back();
        }

        // check if this is a new minimum
        if (f < fmin) {
            fmin = f;
            xmin = x;
            continue;
        }

        // check if this value is higher than the previous one
        if (fprev < f) {
            fprev = f;
            counter++;
        }
    }
    // if counter == 4 the function is monotonically increasing to the right, and we shouldn't explore it further
    bool right = !(counter == 4);

    // we now change tactics: instead of requiring 3 monotonic increases in fval before stopping, we now just want it to be higher than the mean four times in a row
    auto points = get_evaluated_points();    
    double mu = points.mean();

    if (right) {
        // now go the remaining steps to the right, terminating if four consecutive evals are all above the mean
        counter = 0;
        x = xmid + 4*spacing;   // start four steps to the right of the middle
        unsigned int above = 0; // number of consecutive points higher than the mean
        while (above < 4 && counter++ < (evals-9)/2) {
            x += spacing;
            double f = function(&x);
            if (f < fmin) {
                fmin = f;
                xmin = x;
            }

            if (mu < f) {
                above++;
            } else {
                above = 0;
            }
        }
    }

    if (left) {
        // repeat for left-steps
        counter = 0;
        unsigned int above = 0;
        x = xmid - 4*spacing;   // start four steps to the left of the middle
        while (above < 4 && counter++ < (evals-9)/2) {
            x -= spacing;
            double f = function(&x);
            if (f < fmin) {
                fmin = f;
                xmin = x;
            }

            if (mu < f) {
                above++;
            } else {
                above = 0;
            }
        }
    }

    return get_evaluated_points();
}

Dataset2D MinimumExplorer::get_evaluated_points() const {
    if (evaluations.empty()) {throw except::bad_order("Error in MinimumExplorer::get_evaluated_points: Cannot get evaluated points before a minimization call has been made.");}

    unsigned int N = evaluations.size();
    std::vector<double> x(N), y(N);
    for (unsigned int i = 0; i < N; i++) {
        x[i] = evaluations[i].vals[0];
        y[i] = evaluations[i].fval;
    }
    return Dataset2D(x, y, "x", "f(x)");
}

Result MinimumExplorer::minimize_override() {
    auto l = landscape(evals);
    auto min = l.find_minimum();
    FittedParameter p(parameters[0], min.x, l.span_x() - min.x);
    return Result(p, l.mean(), fevals);
}

void MinimumExplorer::add_parameter(const Parameter& param) {
    if (!param.has_guess()) {throw except::invalid_operation("Error in MinimumExplorer::add_parameter: Guess value must be supplied.");}
    if (!parameters.empty()) {throw except::invalid_operation("Error in MinimumExplorer::add_parameter: This minimizer only supports 1D problems.");}
    parameters.push_back(param);
}