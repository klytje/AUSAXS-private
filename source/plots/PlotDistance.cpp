#include <plots/PlotDistance.h>
#include <plots/PlotDataset.h>
#include <hist/ScatteringHistogram.h>

plots::PlotDistance::~PlotDistance() = default;

plots::PlotDistance::PlotDistance(const hist::ScatteringHistogram& d, const io::File& path) {
    quick_plot(d, path);
}

void plots::PlotDistance::quick_plot(const hist::ScatteringHistogram& d, const io::File& path) {
    auto distances = d.axis.as_vector();
    SimpleDataset p(distances, d.p.data);
    SimpleDataset pp(distances, d.p_pp.p.data);
    SimpleDataset ph(distances, d.p_hp.p.data);
    SimpleDataset hh(distances, d.p_hh.p.data);

    p.add_plot_options("lines", {{"color", style::color::black}, {"legend", "total"}, {"xlabel", "Distance [$\\AA$]"}, {"ylabel", "Count"}});
    pp.add_plot_options("lines", {{"color", style::color::orange}, {"legend", "atom-atom"}});
    ph.add_plot_options("lines", {{"color", style::color::green}, {"legend", "atom-water"}});
    hh.add_plot_options("lines", {{"color", style::color::blue}, {"legend", "water-water"}});

    plots::PlotDataset plot;
    plot.plot(p);
    plot.plot(pp);
    plot.plot(ph);
    plot.plot(hh);
    plot.save(path);
}
