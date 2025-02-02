#pragma once

#include <io/IOFwd.h>
#include <fitter/Fit.h>
#include <utility/observer_ptr.h>

#include <string>
#include <functional>

namespace fitter {
    class FitReporter {
        public:
            template<FitType T>
            static void report(const T& fit);

            template<FitType T>
            static void report(const observer_ptr<T> fit);

            template<FitType T>
            static void report(const std::vector<T>& fits, const std::vector<std::string>& titles = {});

            template<FitType T>
            static void save(const T& fit, const io::File& path);

            template<FitType T>
            static void save(const observer_ptr<T> fit, const io::File& path);

            template<FitType T>
            static void save(const observer_ptr<T> fit, const io::File& path, const std::string& header);

            template<FitType T>
            static void save(const std::vector<T>& fits, const io::File& path, const std::vector<std::string>& titles = {});

    private:
            [[nodiscard]] static std::function<std::string(std::string)> get_title_reporter(const std::vector<std::string>& titles); 
    };
}