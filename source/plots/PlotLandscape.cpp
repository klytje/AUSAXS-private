#include <plots/PlotLandscape.h>

plots::PlotLandscape::PlotLandscape(const mini::Landscape& data, std::string path) {
    ss << "PlotLandscape\n" 
        << data.to_string() 
        << "\n"
        << data.get_plot_options().to_string() 
        << std::endl;

    save(path);
}

plots::PlotLandscape::~PlotLandscape() = default;

void plots::PlotLandscape::quick_plot(const mini::Landscape& data, std::string path) {
    PlotLandscape plot(data, path);
}