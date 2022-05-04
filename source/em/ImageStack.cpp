#include <algorithm>
#include <fstream>

//! ##### Remove ##### !
#include <chrono>
using namespace std::chrono;
//! ################## !

#include <em/NoCulling.h>
#include <em/CounterCulling.h>
#include <em/ImageStack.h>
#include <em/PartialHistogramManager.h>
#include <data/Atom.h>
#include <data/Protein.h>
#include <fitter/SimpleIntensityFitter.h>
#include <plots/PlotIntensityFit.h>
#include <plots/PlotIntensityFitResiduals.h>
#include <utility/Exceptions.h>

#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>

using namespace setting::em;
using namespace em;

ImageStack::ImageStack(const vector<Image>& images, unsigned int resolution, CullingStrategyChoice csc) 
    : size_x(images[0].N), size_y(images[0].M), size_z(images.size()), phm(std::make_unique<em::PartialHistogramManager>(*this)) {

    data = images;
    setup(csc);
}

ImageStack::ImageStack(string file, unsigned int resolution, CullingStrategyChoice csc) 
    : filename(file), header(std::make_shared<ccp4::Header>()), resolution(resolution), phm(std::make_unique<em::PartialHistogramManager>(*this)) {

    std::ifstream input(file, std::ios::binary);
    if (!input.is_open()) {throw except::io_error("Error in ImageStack::ImageStack: Could not open file \"" + file + "\"");}

    input.read(reinterpret_cast<char*>(header.get()), sizeof(*header));
    read(input, get_byte_size());
    setup(csc);
}

ImageStack::~ImageStack() = default;

void ImageStack::setup(CullingStrategyChoice csc) {
    determine_staining();
    if (negatively_stained()) {std::transform(charge_levels.begin(), charge_levels.end(), charge_levels.begin(), std::negate<double>());}

    switch (csc) {
        case CullingStrategyChoice::NoStrategy:
            culler = std::make_unique<NoCulling>();
            break;
        case CullingStrategyChoice::CounterStrategy:
            culler = std::make_unique<CounterCulling>();
            break;
        default: 
            throw except::unknown_argument("Error in Grid::Grid: Unkown PlacementStrategy");
    }
}

void ImageStack::save(string path, double cutoff) const {
    std::shared_ptr<Protein> protein = phm->get_protein(cutoff);
    protein->save(path);
}

Image& ImageStack::image(unsigned int layer) {return data[layer];}

const Image& ImageStack::image(unsigned int layer) const {return data[layer];}

size_t ImageStack::size() const {return size_z;}

const vector<Image>& ImageStack::images() const {return data;}

std::unique_ptr<Grid> ImageStack::create_grid(double) const {
    std::cout << "Error in ImageStack::create_grid: Not implemented yet. " << std::endl;
    exit(1);
}

histogram::ScatteringHistogram ImageStack::get_histogram(double cutoff) const {
    return phm->get_histogram(cutoff);
}

histogram::ScatteringHistogram ImageStack::get_histogram(const std::shared_ptr<EMFit> res) const {
    return get_histogram(res->params.at("cutoff"));
}

std::shared_ptr<ImageStack::EMFit> ImageStack::fit(const histogram::ScatteringHistogram& h) {
    SimpleIntensityFitter fitter(h, get_limits());
    determine_minimum_bounds();
    return fit_helper(fitter);
}

std::shared_ptr<ImageStack::EMFit> ImageStack::fit(string filename) {
    SimpleIntensityFitter fitter(filename);
    determine_minimum_bounds();
    return fit_helper(fitter);
}

std::shared_ptr<ImageStack::EMFit> ImageStack::fit_helper(SimpleIntensityFitter& fitter) {
    // convert the calculated intensities to absolute scale
    auto protein = phm->get_protein(1);
    double c = 5;                                                         // concentration
    double m = protein->get_absolute_mass()*constants::unit::mg;          // mass
    double DrhoV2 = std::pow(protein->get_relative_charge(), 2);          // charge
    double re2 = pow(constants::radius::electron*constants::unit::cm, 2); // squared scattering length
    double I0 = DrhoV2*re2*c/m;
    fitter.normalize_intensity(I0);

    // fit function
    unsigned int counter = 0;
    Dataset evaluated_points("cutoff", "chi2");
    std::function<double(const double*)> chi2 = [&] (const double* params) {
        fitter.set_scattering_hist(get_histogram(params[0]));
        double val = fitter.fit()->chi2;

        evaluated_points.x.push_back(params[0]);
        evaluated_points.y.push_back(val);
        std::cout << "Step " << counter++ << ": Evaluated cutoff value " << params[0] << " with chi2 " << val << std::endl;
        return val;
    }; 

    // perform the fit
    ROOT::Math::Functor functor = ROOT::Math::Functor(chi2, 1);
    ROOT::Math::Minimizer* minimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "migrad"); 
    minimizer->SetFunction(functor);

    if (positively_stained()) {minimizer->SetLimitedVariable(0, "cutoff", 2, 1, 1, 10);}
    else {minimizer->SetLimitedVariable(0, "cutoff", -2, 1, -1, -10);}

    minimizer->SetStrategy(2);
    minimizer->SetPrintLevel(2);
    minimizer->Minimize();

    const double* result = minimizer->X();
    std::shared_ptr<EMFit> emfit = std::make_shared<EMFit>(fitter, minimizer, chi2(result));
    emfit->evaluated_points = evaluated_points;

    return emfit;
}

Dataset ImageStack::cutoff_scan(const Axis& points, const histogram::ScatteringHistogram& h) {
    vector<double> cutoffs;
    vector<double> chi2;
    cutoffs.reserve(points.bins);
    chi2.reserve(points.bins);

    unsigned int count = 1;
    double step = points.step();
    for (double cutoff = points.max; cutoff > points.min; cutoff -= step) {
        cutoffs.push_back(cutoff);

        SimpleIntensityFitter fitter(h, get_limits());
        determine_minimum_bounds();
        fitter.set_scattering_hist(get_histogram(cutoff));
        double val = fitter.fit()->chi2;

        chi2.push_back(val);
        std::cout << "\t" << count++ << ": Evaluated cutoff value " << cutoff << " with chi2 " << val << std::endl;
    }

    return Dataset(cutoffs, chi2, "cutoff", "chi2");
}

size_t ImageStack::get_byte_size() const {
    return header->get_byte_size();
}

void ImageStack::read(std::ifstream& istream, size_t byte_size) {
    size_x = header->nx; size_y = header->ny; size_z = header->nz;

    data = vector<Image>(size_z, Image(header));
    for (unsigned int i = 0; i < size_x; i++) {
        for (unsigned int j = 0; j < size_y; j++) {
            for (unsigned int k = 0; k < size_z; k++) {
                istream.read(reinterpret_cast<char*>(&index(i, j, k)), byte_size);
            }
        }
    }

    // set z values
    for (unsigned int z = 0; z < size_z; z++) {
        image(z).set_z(z);
    }
}

float& ImageStack::index(unsigned int x, unsigned int y, unsigned int layer) {
    return data[layer].index(x, y);
}

float ImageStack::index(unsigned int x, unsigned int y, unsigned int layer) const {
    return data[layer].index(x, y);
}

std::shared_ptr<ccp4::Header> ImageStack::get_header() const {
    return header;
}

Limit ImageStack::get_limits() const {
    return resolution == 0 ? Limit(setting::fit::q_low, setting::fit::q_high) : Limit(setting::fit::q_low, 2*M_PI/resolution);
}

double ImageStack::mean() const {
    double sum = 0;
    for (unsigned int z = 0; z < size_z; z++) {
        sum += image(z).mean();
    }
    return sum/size_z;
}

bool ImageStack::positively_stained() const {return staining > 0;}

bool ImageStack::negatively_stained() const {return staining < 0;}

void ImageStack::determine_staining() {
    // we count how many images where the maximum density is positive versus negative
    double sign = 0;
    for (unsigned int z = 0; z < size_z; z++) {
        Limit limit = image(z).limits();
        double min = std::abs(limit.min), max = std::abs(limit.max);
        if (1 <= min && max+1 < min) {
            sign--;
        } else if (1 <= max && min+1 < max) {
            sign++;
        }
    }

    // set the staining type so we don't have to calculate it again later
    if (sign > 0) {staining = 1;}
    else {staining = -1;}
}

ObjectBounds3D ImageStack::minimum_volume(double cutoff) {
    ObjectBounds3D bounds(size_x, size_y, size_z);
    for (unsigned int z = 0; z < size_z; z++) {
        bounds[z] = image(z).setup_bounds(cutoff);
    }

    return bounds;
}

void ImageStack::determine_minimum_bounds() {
    double cutoff = positively_stained() ? 1 : -1;
    std::for_each(data.begin(), data.end(), [&cutoff] (Image& image) {image.setup_bounds(cutoff);});
}