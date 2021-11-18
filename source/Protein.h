#pragma once

// includes
#include <vector>
#include <map>
#include <utility>

// ROOT
#include <TVector3.h>

// my own includes
#include "data/Atom.h"
#include "hydrate/Grid.h"
#include "data/File.h"
#include "data/properties.h"

using std::vector, std::string, std::unique_ptr;
using namespace ROOT;

// static_assert(boost::is_pod<Distances>::value);
struct Distances {
    Distances(vector<double>& pp, vector<double>& hh, vector<double>& hp, 
        vector<double>& wpp, vector<double>& whh, vector<double>& whp)
         : pp(pp), hh(hh), hp(hp), wpp(wpp), whh(whh), whp(whp) {}

    const vector<double> pp, hh, hp;
    const vector<double> wpp, whh, whp;
};

class Protein {
public:
    /** Creates a new protein from the input .pdb or .xml file. 
     * @param path path to the input file. 
     */
    Protein(string path);

    /** Writes this protein to disk.
     * @param path path to the destination. 
     */
    void save(string path);

    /** Calculate the distances between each pair of atoms. 
     * @return A tuple (pp, hh, hp) where pp is all internal distances between the protein atoms, hh is all internal
     * distances between hydration atoms, and hp is all distances between protein atoms and hydration atoms.
     */
    Distances calc_distances();

    /** 
     * @brief Use an algorithm to generate a new hydration layer for this protein. Note that the previous one will be deleted.
     * @param reduce the factor to reduce the output number of water molecules by. 
     * @param width the distance between each grid point
     */
    void generate_new_hydration(int reduce, double width);

    /**
     * @brief Get a pointer to the protein atoms.
     */
    vector<shared_ptr<Atom>>* get_protein_atoms() {return &protein_atoms;}

    /**
     * @brief Get a pointer to the hydration atoms.
     */
    vector<shared_ptr<Hetatom>>* get_hydration_atoms() {return &hydration_atoms;}

    /** Generate a 3D grid containing all atoms.
     * @param width the bin width
     * @return A pair (corner point, bins) where the first is a bottom corner of the grid, while the second is the number of bins in each dimension.
     */
    std::pair<TVector3, vector<int>> generate_grid(const double width);

    /** Calculate the center-mass coordinates for the protein.
     * @return The center-mass (x, y, z) coordinates. 
     */
    TVector3 get_cm();

    /**
     * @brief Calculate the volume of this protein based on its constituent amino acids
     */
    double get_volume();

private:
    vector<shared_ptr<Atom>> protein_atoms; // atoms of the protein itself
    vector<shared_ptr<Hetatom>> hydration_atoms; // hydration layer
    shared_ptr<File> file; 

    /** Move the entire protein by a vector
     * @param v the translation vector
     */
    void translate(const TVector3 v);
};