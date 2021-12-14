#pragma once

// forwards declaration
class ScatteringHistogram;

// includes
#include <vector>
#include <map>
#include <utility>

// my own includes
#include "data/Atom.h"
#include "hydrate/Grid.h"
#include "io/File.h"
#include "constants.h"
#include "ScatteringHistogram.h"

using std::vector, std::string, std::unique_ptr;

class Body {
public:
    /** Create a new collection of atoms (body) from the input .pdb or .xml file. 
     * @param path path to the input file. 
     */
    Body(const string& path) : file(std::make_shared<File>(path)), protein_atoms(file->protein_atoms), hydration_atoms(file->hydration_atoms) {}

    /**
     * @brief Create a new collection of atoms (body) based on two vectors
     */
    Body(const vector<Atom>& protein_atoms, const vector<Hetatom>& hydration_atoms) 
        : file(std::make_unique<File>(protein_atoms, hydration_atoms)), protein_atoms(file->protein_atoms), hydration_atoms(file->hydration_atoms){}

    /** 
     * @brief Writes this body to disk.
     * @param path path to the destination. 
     */
    void save(string path);

    /**
     * @brief Get the distances between each atom.
     */
    shared_ptr<ScatteringHistogram> get_distances();

    /** 
     * @brief Use an algorithm to generate a new hydration layer for this body. Note that the previous one will be deleted.
     */
    void generate_new_hydration();

    /**
     * @brief Get a reference to the constituent atoms.
     */
    const vector<Atom>& get_protein_atoms() {return protein_atoms;}

    /**
     * @brief Get a referece to the hydration atoms.
     */
    const vector<Hetatom>& get_hydration_atoms() {return hydration_atoms;}

    /** 
     * @brief Calculate the center-mass coordinates for the body.
     * @return The center-mass (x, y, z) coordinates. 
     */
    Vector3 get_cm() const;

    /**
     * @brief Calculate the volume of this body based on its constituent amino acids
     */
    double get_volume_acids() const;

    /**
     * @brief Calculate the volume of this body based on the number of grid bins it spans
     */
    double get_volume_grid();

    /**
     * @brief Calculate the volume of this body based on the number of C-alpha atoms
     */
    double get_volume_calpha() const;

    /**
     * @brief Get the grid representation of this body. 
     */
    shared_ptr<Grid> get_grid() const {
        if (grid == nullptr) {
            print_err("Error in Protein::get_grid: Grid has not been instantiated!"); 
            exit(1);
        }
        return grid;
    }

    /**
     * @brief Generate a PDB file at @p path showing the filled grid volume.
     */
    void generate_volume_file(string path);

    /**
     * @brief Calculate the total mass of this body in Daltons.
     */
    double get_mass() const;

    /**
     * @brief Create a grid and fill it with the atoms of this body. 
     */
    void create_grid();

    /** 
     * @brief Move the entire body by a vector.
     * @param v the translation vector
     */
    void translate(const Vector3& v);

    /**
     * @brief Rotatate all atoms @a rad radians about the axis @a axis. 
     * @param axis the rotation axis. 
     * @param rad the amount to rotate in radians. 
     */
    void rotate(const Vector3& axis, const double& rad);

    /**
     * @brief Euler angle rotation of all atoms. 
     * @param alpha radians to rotate about the z-axis.
     * @param beta radians to rotate about the y-axis. 
     * @param gamma radians to rotate about the x-axis. 
     */
    void rotate(const double& alpha, const double& beta, const double& gamma);

    /** 
     * @brief Calculate the distances between each pair of atoms. 
     */
    void calc_distances();

    /**
     * @brief Subtract the charge of the displaced water molecules from the effective charge of the protein atoms. 
     */
    void update_effective_charge();

    /**
     * @brief Subtract the charge of the displaced water molecules from the effective charge of the protein atoms. 
     * @param charge the charge to be subtracted.
     */
    void update_effective_charge(const double& charge);

    /**
     * @brief Assignment operator overload
     */
    Body& operator=(const Body& body) {
        protein_atoms = body.protein_atoms;
        hydration_atoms = body.hydration_atoms;
        file = body.file;
        grid = body.grid;
        distances = body.distances;
        return *this;
    }

private:
    shared_ptr<File> file = nullptr; // the file backing this body
    shared_ptr<Grid> grid = nullptr; // the grid representation of this body
    shared_ptr<ScatteringHistogram> distances = nullptr; // an object representing the distances between atoms

public: 
    vector<Atom>& protein_atoms; // atoms of the body itself
    vector<Hetatom>& hydration_atoms; // hydration layer
};