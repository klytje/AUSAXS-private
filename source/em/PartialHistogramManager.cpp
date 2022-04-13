#include <vector>

#include <em/PartialHistogramManager.h>
#include <em/ImageStack.h>
#include <em/CounterCulling.h>
#include <ScatteringHistogram.h>
#include <data/Protein.h>

em::PartialHistogramManager::PartialHistogramManager(const ImageStack& images) :images(images), culler(std::make_unique<CounterCulling>()) {}

ScatteringHistogram em::PartialHistogramManager::get_histogram(double cutoff) {
    update_protein(cutoff);
    return protein->get_histogram();
}

std::vector<Atom> em::PartialHistogramManager::generate_atoms(double cutoff) const {
    // we use a list since we will have to append quite a few other lists to it
    std::list<Atom> atoms;
    for (const Image& image: images.images()) {
        std::list<Atom> im_atoms = image.generate_atoms(cutoff);
        atoms.splice(atoms.end(), im_atoms); // move im_atoms to end of atoms
    }

    // convert list to vector
    return culler->cull(atoms);
}

std::unique_ptr<Protein> em::PartialHistogramManager::generate_protein(double cutoff) const {
    vector<Atom> atoms = generate_atoms(cutoff);

    std::function<bool(double, double)> compare_positive = [] (double v1, double v2) {return v1 < v2;};
    std::function<bool(double, double)> compare_negative = [] (double v1, double v2) {return v1 > v2;};
    std::function<bool(double, double)> compare_func = 0 <= cutoff ? compare_positive : compare_negative;

    // sort vector so we can slice it into levels of charge density
    auto comparator = [&compare_func] (const Atom& atom1, const Atom& atom2) {return compare_func(atom1.occupancy, atom2.occupancy);};
    std::sort(atoms.begin(), atoms.end(), comparator);

    unsigned int charge_index = 0, atom_index = 0, current_index = 0;
    double charge = charge_levels[charge_index]; // initialize charge

    vector<Body> bodies(charge_levels.size());
    vector<Atom> current_atoms(atoms.size());
    
    while (compare_func(atoms[atom_index].occupancy, cutoff)) {atom_index++;} // search for first atom with charge larger than the cutoff
    while (compare_func(charge, cutoff)) {std::cout << "Compared " << charge << " with " << cutoff << " (evaluated true)" << std::endl; charge = charge_levels[++charge_index];} // search for first charge level larger than the cutoff 
    for (; atom_index < atoms.size(); atom_index++) {
        if (compare_func(atoms[atom_index].occupancy, charge)) {
            std::cout << atoms[atom_index].occupancy << " compared with " << charge << " evaluated to true!" << std::endl;
            std::cout << "Adding atom " << atom_index << " to body " << charge_index << std::endl;
            current_atoms[current_index++] = atoms[atom_index];
        } else {
            std::cout << "Finished with body " << charge_index << std::endl;
            std::cout << "Adding atom " << atom_index << " to body " << charge_index+1 << std::endl;

            // create the body for this charge bin
            current_atoms.resize(current_index);
            bodies[charge_index] = Body(current_atoms);

            // prepare the next body
            current_index = 1;
            current_atoms.resize(atoms.size());
            current_atoms[0] = atoms[atom_index]; // add the atom of the current iteration

            // increment the charge level
            charge = charge_levels[++charge_index];
        }
    }
    // create the final body of the loop
    current_atoms.resize(current_index);
    bodies[charge_index] = Body(current_atoms);

    return std::make_unique<Protein>(bodies);
}

ScatteringHistogram em::PartialHistogramManager::get_histogram_slow(double cutoff) const {
    return Protein(generate_atoms(cutoff)).get_histogram();
}

void em::PartialHistogramManager::update_protein(double cutoff) {
    if (protein == nullptr || protein->bodies.empty()) {
        protein = generate_protein(cutoff); 
        protein->bind_body_signallers();

        previous_cutoff = cutoff;
        return;
    }
    std::unique_ptr<Protein> new_protein = generate_protein(cutoff);

    std::cout << "\nFAST APPROACH NEW PROTEIN" << std::endl;
    {
        unsigned int i = 0;
        for (const auto& b : new_protein->bodies) {
            std::cout << "BODY " << i++ << std::endl;
            for (const auto& a : b.protein_atoms) {
                std::cout << "\t" << a.as_pdb() << std::endl;
            }
        }
    }

    std::function<bool(double, double)> compare_positive = [] (double v1, double v2) {return v1 < v2;};
    std::function<bool(double, double)> compare_negative = [] (double v1, double v2) {return v1 > v2;};
    std::function<bool(double, double)> compare_func = 0 <= cutoff ? compare_positive : compare_negative;

    if (compare_func(cutoff, previous_cutoff)) {
        // since cutoff is smaller than previously, we have to change all bins in the range [cutoff, previous_cutoff]

        // skip all bins before the relevant range
        unsigned int charge_index = 0;
        double current_cutoff = charge_levels[0];
        while (compare_func(current_cutoff, cutoff) && charge_index < charge_levels.size()) {
            std::cout << "Current cutoff " << current_cutoff << " is smaller than cutoff " << cutoff << ", skipping bin " << charge_index << std::endl;
            current_cutoff = charge_levels[++charge_index];}

        // iterate through the remaining bins, and use a break statement to stop when we leave the relevant range
        for (; charge_index < charge_levels.size()-1; charge_index++) {
            // std::cout << "Charge level " << charge_levels[i] << " compared with previous cutoff " << previous_cutoff << std::endl;
            // check if the current bin is inside the range
            if (compare_func(charge_levels[charge_index], previous_cutoff)) {
                // if so, we replace it with the new contents
                std::cout << "\tReplacing body " << charge_index << std::endl;
                protein->bodies[charge_index] = new_protein->bodies[charge_index];
            } else {
                // otherwise we replace it and stop iterating
                std::cout << "\tReplacing body " << charge_index << std::endl;
                protein->bodies[charge_index] = new_protein->bodies[charge_index];
                break;
            }
        }
    } else {
        // since cutoff is larger than previously, we have to change all bins in the range [previous_cutoff, cutoff]

        // skip all bins before the relevant range
        unsigned int charge_index = 0;
        double current_cutoff = charge_levels[0];
        while (compare_func(current_cutoff, previous_cutoff) && charge_index < charge_levels.size()) {
            std::cout << "Current cutoff " << current_cutoff << " is smaller than previous cutoff " << previous_cutoff << ", skipping bin " << charge_index << std::endl;
            current_cutoff = charge_levels[++charge_index];}

        // iterate through the remaining bins, and use a break statement to stop when we leave the relevant range
        for (; charge_index < charge_levels.size()-1; charge_index++) {
            // std::cout << "Charge level " << charge_levels[i] << " compared with previous cutoff " << previous_cutoff << std::endl;
            // check if the current bin is inside the range
            if (compare_func(charge_levels[charge_index], cutoff)) {
                // if so, we replace it with the new contents
                std::cout << "\tReplacing body " << charge_index << std::endl;
                protein->bodies[charge_index] = new_protein->bodies[charge_index];
            } else {
                // otherwise we replace it and stop iterating
                std::cout << "\tReplacing body " << charge_index << std::endl;
                protein->bodies[charge_index] = new_protein->bodies[charge_index];
                break;
            }
        }

    }

    {
        std::cout << "\nFAST APPROACH FINAL" << std::endl;
        unsigned int i = 0;
        for (const auto& b : protein->bodies) {
            std::cout << "BODY " << i++ << std::endl;
            for (const auto& a : b.protein_atoms) {
                std::cout << "\t" << a.as_pdb() << std::endl;
            }
        }
    }

    previous_cutoff = cutoff;
}

std::shared_ptr<Protein> em::PartialHistogramManager::get_protein() const {return protein;}

void em::PartialHistogramManager::set_cutoff_levels(std::vector<double> levels) {
    charge_levels = levels;
    if (protein != nullptr) {protein->bodies.clear();} // we must reset the bodies to ensure they remain in sync with the new levels
}