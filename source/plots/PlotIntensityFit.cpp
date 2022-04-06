#include "plots/PlotIntensityFit.h"

#include <memory.h>
#include <string.h>
#include <vector>

#include <TLegend.h>
#include <TH1D.h>
#include <TLine.h>

plots::PlotIntensityFit::PlotIntensityFit(SimpleIntensityFitter& fitter) : Plot() {
    prepare_canvas();
    auto graphs = fitter.plot();
    plot(graphs);
}

plots::PlotIntensityFit::PlotIntensityFit(const Fitter::Fit& fit) : Plot() {
    prepare_canvas();
    plot(fit.normal_plot);
}

plots::PlotIntensityFit::PlotIntensityFit(const std::shared_ptr<Fitter::Fit> fit) : Plot() {
    prepare_canvas();
    plot(fit->normal_plot);
}

void plots::PlotIntensityFit::save(std::string path) const {
    canvas->SaveAs(path.c_str());
}

void plots::PlotIntensityFit::plot(const std::vector<std::shared_ptr<TGraph>>& graphs) const {
    // use some nicer colors
    graphs[0]->SetLineColor(kBlack);
    graphs[1]->SetMarkerColor(kBlack);
    graphs[2]->SetMarkerColor(kOrange+1);
    graphs[2]->SetLineColor(kOrange+1);

    graphs[0]->SetMarkerStyle(7);
    graphs[2]->SetMarkerStyle(7);

    // set titles
    graphs[0]->SetTitle("Fit");
    graphs[0]->GetXaxis()->SetTitle("q");
    graphs[0]->GetXaxis()->CenterTitle();
    graphs[0]->GetXaxis()->SetTitleOffset(1.05);
    graphs[0]->GetYaxis()->SetTitle("Intensity");
    graphs[0]->GetYaxis()->CenterTitle();

    // draw the graphs
    graphs[2]->DrawClone("AP"); // Point
    graphs[0]->DrawClone("SAME P"); // Axes points
    graphs[1]->DrawClone("SAME L"); // Line
}

void plots::PlotIntensityFit::prepare_canvas() {
    canvas = std::make_unique<TCanvas>("PlotIntensityCanvas", "canvas", 600, 600);
    linpad = std::make_unique<TPad>("PlotIntensityPad1", "linpad", 0, 0, 1, 1); // create a drawing pad

    linpad->SetLeftMargin(0.19);
    linpad->SetLogx();
    linpad->SetLogy();

    linpad->Draw();
    linpad->cd();

    // canvas->SetLogy();
    // canvas->SetLogx();
    // canvas->SetRightMargin(0.15);
    // canvas->SetLeftMargin(0.15);
}