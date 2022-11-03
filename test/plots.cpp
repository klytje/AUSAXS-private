#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <plots/all.h>

TEST_CASE("plot_dataset", "[plots]") {
    SimpleDataset data("test/files/2epe.dat");
    plots::PlotDataset::quick_plot(data, "temp/plot/dataset.png");
}

TEST_CASE("plot_distance", "[plots]") {
    Protein protein("test/files/2epe.pdb");
    protein.generate_new_hydration();
    auto data = protein.get_histogram();
    plots::PlotDistance::quick_plot(data, "temp/plot/distance.png");
}

TEST_CASE("plot_histogram", "[plots]") {
    
}

TEST_CASE("plot_image", "[plots]") {
    
}

TEST_CASE("plot_intensity", "[plots]") {
    
}

TEST_CASE("plot_intensityfit", "[plots]") {
    
}

TEST_CASE("plot_intensityfitresiduals", "[plots]") {
    
}

TEST_CASE("plot_resolution", "[plots]") {
    
}