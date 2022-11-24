// includes
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

// my own includes
#include <data/Atom.h>
#include <hydrate/Grid.h>
#include <utility/Constants.h>
#include <data/Body.h>
#include <utility/Settings.h>
#include <math/Matrix.h>
#include <math/MatrixUtils.h>

Body::Body() {}

Body::Body(std::string path) : file(path), uid(uid_counter++) {}

Body::Body(const std::vector<Atom>& protein_atoms, const std::vector<Water>& hydration_atoms) : file(protein_atoms, hydration_atoms), uid(uid_counter++) {}

Body::Body(const Body& body) : file(body.file), uid(body.uid) {}

Body::Body(Body&& body) : file(std::move(body.file)), uid(body.uid) {}

Body::~Body() = default;

void Body::save(std::string path) {file.write(path);}

void Body::calc_histogram() {
    // generous sizes - 1000Å should be enough for just about any structure
    double width = setting::axes::scattering_intensity_plot_binned_width;
    Axis axes = Axis(1000/width, 0, 1000); 
    std::vector<double> p_pp(axes.bins, 0);
    std::vector<double> p_hh(axes.bins, 0);
    std::vector<double> p_hp(axes.bins, 0);
    std::vector<double> p_tot(axes.bins, 0);

    // extremely wasteful to calculate this from scratch every time
    std::vector<float> data_p(file.protein_atoms.size()*4);
    for (size_t i = 0; i < file.protein_atoms.size(); i++) {
        const Atom& a = file.protein_atoms[i]; 
        data_p[4*i] = a.coords.x();
        data_p[4*i+1] = a.coords.y();
        data_p[4*i+2] = a.coords.z();
        data_p[4*i+3] = a.effective_charge*a.occupancy;
    }

    std::vector<float> data_h(file.hydration_atoms.size()*4);
    for (size_t i = 0; i < file.hydration_atoms.size(); i++) {
        const Water& a = file.hydration_atoms[i]; 
        data_h[4*i] = a.coords.x();
        data_h[4*i+1] = a.coords.y();
        data_h[4*i+2] = a.coords.z();
        data_h[4*i+3] = a.effective_charge*a.occupancy;
    }

    // calculate p-p distances
    for (size_t i = 0; i < file.protein_atoms.size(); i++) {
        for (size_t j = i+1; j < file.protein_atoms.size(); j++) {
            float weight = data_p[4*i+3]*data_p[4*j+3]; // Z1*Z2*w1*w2
            float dx = data_p[4*i] - data_p[4*j];
            float dy = data_p[4*i+1] - data_p[4*j+1];
            float dz = data_p[4*i+2] - data_p[4*j+2];
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            p_pp[dist/width] += 2*weight;
        }
    }

    // add self-correlation
    for (size_t i = 0; i < file.protein_atoms.size(); i++) {p_pp[0] += data_p[4*i+3]*data_p[4*i+3];}

    for (size_t i = 0; i < file.hydration_atoms.size(); i++) {
        // calculate h-h distances
        for (size_t j = i+1; j < file.hydration_atoms.size(); j++) {
            float weight = data_h[4*i+3]*data_h[4*j+3]; // Z1*Z2*w1*w2
            float dx = data_h[4*i] - data_h[4*j];
            float dy = data_h[4*i+1] - data_h[4*j+1];
            float dz = data_h[4*i+2] - data_h[4*j+2];
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            p_hh[dist/width] += 2*weight;
        }

        // calculate h-p distances
        for (size_t j = 0; j < file.protein_atoms.size(); j++) {
            float weight = data_h[4*i+3]*data_p[4*j+3]; // Z1*Z2*w1*w2
            float dx = data_h[4*i] - data_p[4*j];
            float dy = data_h[4*i+1] - data_p[4*j+1];
            float dz = data_h[4*i+2] - data_p[4*j+2];
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            p_hp[dist/width] += 2*weight;
        }
    }

    // add self-correlation
    for (size_t i = 0; i < file.hydration_atoms.size(); i++) {p_hh[0] += data_h[4*i+3]*data_h[4*i+3];}

    // downsize our axes to only the relevant area
    int max_bin = 10; // minimum size is 10
    for (int i = axes.bins-1; i >= 10; i--) {
        if (p_pp[i] != 0 || p_hh[i] != 0 || p_hp[i] != 0) {
            max_bin = i+1; // +1 since we usually use this for looping (i.e. i < max_bin)
            break;
        }
    }

    p_pp.resize(max_bin);
    p_hh.resize(max_bin);
    p_hp.resize(max_bin);
    p_tot.resize(max_bin);
    axes = Axis{max_bin, 0, max_bin*width}; 

    // calculate p_tot    
    for (int i = 0; i < max_bin; i++) {p_tot[i] = p_pp[i] + p_hh[i] + p_hp[i];}

    histogram = std::make_shared<hist::ScatteringHistogram>(p_pp, p_hh, p_hp, p_tot, axes);
}

void Body::center() {
    if (!centered && setting::protein::center) {
        translate(-get_cm());
        centered = true;
    }
}

Vector3<double> Body::get_cm() const {
    Vector3<double> cm;
    double M = 0; // total mass
    auto weighted_sum = [&cm, &M] (auto& atoms) {
        for (auto const& a : atoms) {
            double m = a.get_mass();
            M += m;
            cm += a.coords*m;
        }
    };
    weighted_sum(file.protein_atoms);
    weighted_sum(file.hydration_atoms);
    return cm/M;
}

double Body::get_volume_acids() const {
    double v = 0;
    int cur_seq = 0; // sequence number of current acid
    for (auto const& a : file.protein_atoms) {
        int a_seq = a.resSeq; // sequence number of current atom
        if (cur_seq != a_seq) { // check if we are still dealing with the same acid
            cur_seq = a_seq; // if not, update our current sequence number
            v += constants::volume::amino_acids.get(a.resName); // and add its volume to the running total
        }
    }
    return v;
}

std::shared_ptr<hist::ScatteringHistogram> Body::get_histogram() {
    if (histogram == nullptr) {calc_histogram();}
    return histogram;
}

void Body::translate(const Vector3<double>& v) {
    changed_external_state();

    std::for_each(file.protein_atoms.begin(), file.protein_atoms.end(), [&v] (Atom& atom) {atom.translate(v);});
    std::for_each(file.hydration_atoms.begin(), file.hydration_atoms.end(), [&v] (Water& atom) {atom.translate(v);});
}

void Body::rotate(const Matrix<double>& R) {
    for (auto& atom : file.protein_atoms) {
        atom.coords.rotate(R);
    }

    for (auto& atom : file.hydration_atoms) {
        atom.coords.rotate(R);
    }
}

void Body::rotate(double alpha, double beta, double gamma) {
    changed_external_state();
    Matrix R = matrix::rotation_matrix(alpha, beta, gamma);
    rotate(R);
}

void Body::rotate(const Vector3<double>& axis, double angle) {
    changed_external_state();
    Matrix R = matrix::rotation_matrix(axis, angle);
    rotate(R);
}

void Body::update_effective_charge(double charge) {
    changed_external_state();
    std::for_each(file.protein_atoms.begin(), file.protein_atoms.end(), [&charge] (Atom& a) {a.add_effective_charge(charge);});
    updated_charge = true;
}

double Body::total_atomic_charge() const {
    return std::accumulate(atoms().begin(), atoms().end(), 0.0, [] (double sum, const Atom& atom) {return sum + atom.Z();});
}

double Body::total_effective_charge() const {
    return std::accumulate(atoms().begin(), atoms().end(), 0.0, [](double sum, const Atom& a) { return sum + a.get_effective_charge(); });
}

double Body::molar_mass() const {
    return absolute_mass()*constants::Avogadro;
}

double Body::absolute_mass() const {
    double M = 0;
    std::for_each(file.protein_atoms.begin(), file.protein_atoms.end(), [&M] (const Atom& a) {M += a.get_mass();});
    std::for_each(file.hydration_atoms.begin(), file.hydration_atoms.end(), [&M] (const Water& a) {M += a.get_mass();});
    return M;
}

Body& Body::operator=(const Body& rhs) {
    file = rhs.file; 
    uid = rhs.uid;
    changed_internal_state();
    return *this;
}

bool Body::operator==(const Body& rhs) const {
    return uid == rhs.uid;
}

void Body::changed_external_state() const {signal->external_change();}

void Body::changed_internal_state() const {signal->internal_change();}

void Body::register_probe(std::shared_ptr<StateManager::BoundSignaller> signal) {this->signal = signal;}

std::vector<Atom>& Body::atoms() {return file.protein_atoms;}

std::vector<Water>& Body::waters() {return file.hydration_atoms;}

const std::vector<Atom>& Body::atoms() const {return file.protein_atoms;}

const std::vector<Water>& Body::waters() const {return file.hydration_atoms;}

Atom& Body::atoms(unsigned int index) {return file.protein_atoms[index];}

const Atom& Body::atoms(unsigned int index) const {return file.protein_atoms[index];}

File& Body::get_file() {return file;}