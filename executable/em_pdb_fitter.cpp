#include <iostream>

#include <plots/all.h>
#include <em/ImageStack.h>
#include <utility/Utility.h>
#include <fitter/FitReporter.h>

using std::string;

int main(int argc, char const *argv[]) {
    setting::protein::use_effective_charge = false;
    setting::fit::verbose = false;
    setting::em::sample_frequency = 2;

    // check that we have at least one argument
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<mapfile> <pdbfile>" << std::endl;
        return 1;
    }

    // load the input files
    string mapfile = argv[1];
    string pdbfile = argv[2];
    setting::plot::path = "figures/em_pdb_fitter/" + utility::stem(mapfile) + "/";
    string mfile = setting::plot::path + "temp.dat";

    // load the map and protein
    em::ImageStack map(mapfile); 
    Protein protein(pdbfile);

    // simulate a SAXS measurement
    SimpleDataset msim = protein.simulate_dataset(false);
    msim.save(mfile);
    auto res = map.fit(mfile);
    FitReporter::report(res);
    FitReporter::save(setting::plot::path + "fit.txt", res);
}