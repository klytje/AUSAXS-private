#pragma once

#include <preprocessor.h>
#include <mini/dlibMinimizer.h>
#include <mini/Golden.h>
#include <mini/MinimumExplorer.h>
#include <mini/Scan.h>
#include <mini/LimitedScan.h>
#include <mini/Utility.h>

#include <memory>
#include <functional>

namespace mini {
    enum class Type {
        BFGS,
        GOLDEN,
        MINIMUM_EXPLORER,
        SCAN,
        LIMITED_SCAN
    };

    namespace detail {
        std::shared_ptr<Minimizer> create_minimizer(Type t) {
            switch (t) {
                case Type::BFGS:
                    return std::make_shared<dlibMinimizer>();
                case Type::GOLDEN:
                    return std::make_shared<Golden>();
                case Type::MINIMUM_EXPLORER:
                    return std::make_shared<MinimumExplorer>();
                case Type::SCAN:
                    return std::make_shared<Scan>();
                case Type::LIMITED_SCAN:
                    return std::make_shared<LimitedScan>();
            }
        }
    }

    std::shared_ptr<Minimizer> create_minimizer(Type t, double(&func)(std::vector<double>)) {
        auto minimizer = detail::create_minimizer(t);
        minimizer->set_function(func);
        return minimizer;
    }

    std::shared_ptr<Minimizer> create_minimizer(Type t, std::function<double(std::vector<double>)> func) {
        auto minimizer = detail::create_minimizer(t);
        minimizer->set_function(func);
        return minimizer;
    }

    std::shared_ptr<Minimizer> create_minimizer(Type t, double(&func)(std::vector<double>), const Parameter& param) {
        auto minimizer = create_minimizer(t, func);
        minimizer->add_parameter(param);
        return minimizer;
    }

    std::shared_ptr<Minimizer> create_minimizer(Type t, std::function<double(std::vector<double>)> func, const Parameter& param) {
        auto minimizer = create_minimizer(t, func);
        minimizer->add_parameter(param);
        return minimizer;
    }

    std::shared_ptr<Minimizer> create_minimizer(Type t, std::function<double(std::vector<double>)> func, const Parameter& param, unsigned int evals) {
        switch (t) {
            case Type::MINIMUM_EXPLORER:
                return std::make_shared<MinimumExplorer>(func, param, evals);
            case Type::SCAN:
                return std::make_shared<Scan>(func, param, evals);
            case Type::LIMITED_SCAN:
                return std::make_shared<LimitedScan>(func, param, evals);
            default:
                debug_print("all::create_minimizer: evals is only used for MinimumExplorer, Scan, and LimitedScan.");
                return create_minimizer(t, func, param);
        }
    }
}