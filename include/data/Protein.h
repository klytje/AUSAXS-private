// includes
#include <string>
#include <vector>
#include <utility>

// my own stuff
#include "data/Body.h"
#include "Tools.h"
#include "data/Atom.h"
#include "data/Hetatom.h"

using std::vector, std::string;

class Protein {
public: 
    Protein(const vector<Atom>& protein_atoms, const vector<Hetatom>& hydration_atoms);
    Protein(const string& input);
    Protein() {}

    /** 
     * @brief Writes this body to disk.
     * @param path path to the destination. 
     */
    void save(string path);

    /** 
     * @brief Use an algorithm to generate a new hydration layer for this body. Note that the previous one will be deleted.
     */
    void generate_new_hydration();

    /**
     * @brief Calculate the volume of this protein based on its constituent amino acids
     */
    double get_volume_acids() const;

    /**
     * @brief Calculate the volume of this protein based on the number of grid bins it spans
     */
    double get_volume_grid();

    /**
     * @brief Calculate the volume of this protein based on the number of C-alpha atoms
     */
    double get_volume_calpha() const;

    /** 
     * @brief Calculate the center-mass coordinates for the protein.
     * @return The center-mass (x, y, z) coordinates. 
     */
    Vector3 get_cm() const;

    /**
     * @brief Calculate the total mass of this protein in Daltons.
     */
    double get_mass() const;

    /**
     * @brief Get a copy of all protein atoms from the underlying bodies.
     */
    vector<Atom> get_protein_atoms() const;

    /**
     * @brief Get a copy of the hydration atoms. Use the member variable for reference access. 
     */
    vector<Hetatom> get_hydration_atoms() const;

private:
    vector<Body> bodies;
    vector<Hetatom> hydration_atoms;
    shared_ptr<Grid> grid = nullptr; // the grid representation of this body

    /** 
     * @brief Move the entire protein by a vector.
     * @param v the translation vector.
     */
    void translate(const Vector3& v);

    /**
     * @brief Create a grid and fill it with the atoms of this protein. 
     */
    void create_grid();

    /** 
     * @brief Calculate the distances between each pair of atoms. 
     */
    void calc_distances();

    /**
     * @brief Subtract the charge of the displaced water molecules from the effective charge of the protein atoms. 
     */
    void update_effective_charge();
};