// includes
#include <vector>
#include <string>
#include <iostream>

#include "Protein.h"
#include "fitter/IntensityFitter.cpp"

#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLine.h>

using std::cout, std::endl;

int main(int argc, char const *argv[]) {
    setting::grid::psc = setting::grid::RadialStrategy;

    Protein protein(argv[1]);
    protein.generate_new_hydration();
    std::shared_ptr<Distances> d = protein.get_distances();
    d->set_axes({60, 0, 60});
    auto I = d->calc_debye_scattering_intensity();
    std::vector<double> q = d->get_xaxis();

    IntensityFitter fitter(argv[2], q, I);
    Fitter::Fit result = fitter.fit();

//*** FIT PLOT ***//
    std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1", "canvas", 600, 600);
    auto graphs = fitter.plot();

    // use some nicer colors
    graphs[0]->SetLineColor(kBlack);
    graphs[1]->SetMarkerColor(kBlack);
    graphs[2]->SetMarkerColor(kOrange+1);
    graphs[2]->SetLineColor(kOrange+1);

    graphs[0]->SetMarkerStyle(7);
    graphs[2]->SetMarkerStyle(7);

    // draw the graphs
    graphs[0]->Draw("AP"); // Axes Line
    graphs[1]->Draw("SAME L"); // Point
    graphs[2]->Draw("SAME P"); // Point

    // setup the canvas and save the plot
    string path = string(argv[3]) + "intensity_fit.pdf";
    c1->SetTitle("Fit");
    c1->SetLogy();
    c1->SetLogx();
    c1->SetRightMargin(0.15);
    c1->SetLeftMargin(0.15);
    c1->SaveAs(path.c_str());

//*** RESIDUAL PLOT ***//
    std::unique_ptr<TCanvas> c2 = std::make_unique<TCanvas>("c2", "canvas", 600, 600);
    std::unique_ptr<TGraphErrors> graph = fitter.plot_residuals();
    std::unique_ptr<TLine> line = std::make_unique<TLine>(0, 0, 1, 0); // solid black line at x=0

    // use some nicer colors
    graph->SetMarkerColor(kOrange+1);
    graph->SetLineColor(kOrange+1);
    line->SetLineColor(kBlack);

    graph->SetMarkerStyle(7);

    // draw the graphs
    graph->Draw("AP");
    line->Draw("SAME");

    // setup the canvas and save the plot
    path = string(argv[3]) + "residuals.pdf";
    c2->SetTitle("Residuals");
    c2->SetLogx();
    c2->SetRightMargin(0.15);
    c2->SetLeftMargin(0.15);
    c2->SaveAs(path.c_str());

    cout << "Result is " << result.params["k"] << "." << endl;
    cout << "c is: " << result.params["k"]*protein.get_mass()*constants::unit::gm/pow(constants::radius::electron*constants::unit::cm, 2) << endl;
    cout << "Chi2: " << result.chi2 << ", dof: " << result.dof << endl;
    cout << "Chi2/dof: " << result.chi2/result.dof << endl;
    return 0;
}