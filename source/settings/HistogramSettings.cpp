#include <settings/HistogramSettings.h>
#include <settings/SettingsIORegistry.h>
#include <utility/Exceptions.h>
#include <utility/StringUtils.h>

unsigned int settings::axes::max_distance = 2000;
double settings::axes::distance_bin_width = 0.2;
unsigned int settings::axes::bins = 1000;
double settings::axes::qmin = 1e-4;
double settings::axes::qmax = 0.5;
unsigned int settings::axes::skip = 0;

namespace settings::axes::io {
    settings::io::SettingSection axes_settings("Axes", {
        settings::io::create(max_distance, "max_distance"),
        settings::io::create(distance_bin_width, "distance_bin_width"),
        settings::io::create(bins, "bins"),
        settings::io::create(qmin, "qmin"),
        settings::io::create(qmax, "qmax"),
        settings::io::create(skip, "skip"),
    });
}

settings::hist::HistogramManagerChoice settings::hist::histogram_manager = settings::hist::HistogramManagerChoice::PartialHistogramManagerMT;
settings::io::SettingSection hist_settings("Histogram", {
    settings::io::create(settings::hist::histogram_manager, "histogram_manager")
});

template<> std::string settings::io::detail::SettingRef<settings::hist::HistogramManagerChoice>::get() const {
    switch (settingref) {
        case settings::hist::HistogramManagerChoice::HistogramManager: return "hm";
        case settings::hist::HistogramManagerChoice::HistogramManagerMT: return "hmmt";
        case settings::hist::HistogramManagerChoice::HistogramManagerMTFF: return "hmmtff";
        case settings::hist::HistogramManagerChoice::PartialHistogramManager: return "phm";
        case settings::hist::HistogramManagerChoice::PartialHistogramManagerMT: return "phmmt";
        case settings::hist::HistogramManagerChoice::PartialHistogramManagerMTFF: return "phmmtff";
        case settings::hist::HistogramManagerChoice::DebugManager: return "debug";
        default: return std::to_string(static_cast<int>(settingref));
    }
}

template<> void settings::io::detail::SettingRef<settings::hist::HistogramManagerChoice>::set(const std::vector<std::string>& val) {
    if (utility::to_lowercase(val[0]) == "hm") {settingref = settings::hist::HistogramManagerChoice::HistogramManager;}
    else if (utility::to_lowercase(val[0]) == "hmmt") {settingref = settings::hist::HistogramManagerChoice::HistogramManagerMT;}
    else if (utility::to_lowercase(val[0]) == "hmmtff") {settingref = settings::hist::HistogramManagerChoice::HistogramManagerMTFF;}
    else if (utility::to_lowercase(val[0]) == "phm") {settingref = settings::hist::HistogramManagerChoice::PartialHistogramManager;}
    else if (utility::to_lowercase(val[0]) == "phmmt") {settingref = settings::hist::HistogramManagerChoice::PartialHistogramManagerMT;}
    else if (utility::to_lowercase(val[0]) == "phmmtff") {settingref = settings::hist::HistogramManagerChoice::PartialHistogramManagerMTFF;}
    else if (utility::to_lowercase(val[0]) == "debug") {settingref = settings::hist::HistogramManagerChoice::DebugManager;}
    else if (!val[0].empty() && std::isdigit(val[0][0])) {settingref = static_cast<settings::hist::HistogramManagerChoice>(std::stoi(val[0]));}
    else {
        throw except::io_error("settings::hist::histogram_manager: Unkown HistogramManagerChoice. Did you forget to add parsing support for it in HistogramSettings.cpp?");
    }
}