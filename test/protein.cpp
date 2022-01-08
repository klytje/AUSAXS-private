// includes
#include <vector>
#include <string>
#include <fstream>

#include "data/Protein.h"
#include "hydrate/Grid.h"
#include "constants.h"

#include "catch2/catch.hpp"

bool approx(double v1, double v2) {
    double tol = 1e-6;
    if (v1-tol > v2) {return false;}
    if (v1+tol < v2) {return false;}
    return true;
}

TEST_CASE("histogram", "[protein]") {
    SECTION("atoms only") {
    //*** TEST ATOMS ***//
        // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
        vector<Atom> b1 = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1)};
        vector<Atom> b2 = {Atom(Vector3(1, -1, -1), 1, "C", "C", 1), Atom(Vector3(1, 1, -1), 1, "C", "C", 1)};
        vector<Atom> b3 = {Atom(Vector3(-1, -1, 1), 1, "C", "C", 1), Atom(Vector3(-1, 1, 1), 1, "C", "C", 1)};
        vector<Atom> b4 = {Atom(Vector3(1, -1, 1), 1, "C", "C", 1), Atom(Vector3(1, 1, 1), 1, "C", "C", 1)};
        vector<vector<Atom>> atoms = {b1, b2, b3, b4};
        Protein protein(atoms, {});

        // set the weights to 1 so we can analytically determine the result
        for (const auto& body : protein.bodies) {
            for (auto& atom : body.protein_atoms) {
                atom.set_effective_charge(1);
            }
        }
        protein.updated_charge = true;

        // calculate the histogram
        shared_ptr<ScatteringHistogram> hist = protein.get_histogram();
        const vector<double> d = hist->p_tot;

        // calculation: 8 identical points. 
        //      each point has:
        //          1 line  of length 0
        //          3 lines of length 2
        //          3 lines of length sqrt(2*2^2) = sqrt(8) = 2.82
        //          1 line  of length sqrt(3*2^2) = sqrt(12) = 3.46
        const vector<double> d_exp = {8, 0, 2*8*3, 8, 0, 0, 0, 0, 0, 0};
        for (size_t i = 0; i < d_exp.size(); i++) {
            if (d[i] != d_exp[i]) {
                cout << "Failed on index " << i << ". Values: " << d[i] << ", " << d_exp[i] << endl;
                REQUIRE(false);
            }
        }
    }

    SECTION("waters only") {
        // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
        vector<Atom> a = {};
        vector<Hetatom> w = {Hetatom(Vector3(-1, -1, -1), 1, "C", "C", 1), Hetatom(Vector3(-1, 1, -1), 1, "C", "C", 1), 
                            Hetatom(Vector3(1, -1, -1), 1, "C", "C", 1),  Hetatom(Vector3(1, 1, -1), 1, "C", "C", 1), 
                            Hetatom(Vector3(-1, -1, 1), 1, "C", "C", 1),  Hetatom(Vector3(-1, 1, 1), 1, "C", "C", 1),
                            Hetatom(Vector3(1, -1, 1), 1, "C", "C", 1),   Hetatom(Vector3(1, 1, 1), 1, "C", "C", 1)};
        Protein protein(a, w);

        // set the weights to 1 so we can analytically determine the result
        for (auto& atom : protein.hydration_atoms) {
            atom.set_effective_charge(1);
        }
        protein.updated_charge = true;

        // calculate the histogram
        shared_ptr<ScatteringHistogram> hist = protein.get_histogram();
        const vector<double> d = hist->p_tot;

        // calculation: 8 identical points. 
        //      each point has:
        //          1 line  of length 0
        //          3 lines of length 2
        //          3 lines of length sqrt(2*2^2) = sqrt(8) = 2.82
        //          1 line  of length sqrt(3*2^2) = sqrt(12) = 3.46
        const vector<double> d_exp = {8, 0, 2*8*3, 8, 0, 0, 0, 0, 0, 0};
        for (size_t i = 0; i < d_exp.size(); i++) {
            if (d[i] != d_exp[i]) {
                cout << "Failed on index " << i << ". Values: " << d[i] << ", " << d_exp[i] << endl;
                REQUIRE(false);
            }
        }
    }

    SECTION("both atoms & water") {
        // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
        vector<Atom> b1 = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1)};
        vector<Atom> b2 = {Atom(Vector3(1, -1, -1), 1, "C", "C", 1), Atom(Vector3(1, 1, -1), 1, "C", "C", 1)};
        vector<Atom> b3 = {Atom(Vector3(-1, -1, 1), 1, "C", "C", 1), Atom(Vector3(-1, 1, 1), 1, "C", "C", 1)};
        vector<Hetatom> w = {Hetatom(Vector3(1, -1, 1), 1, "C", "C", 1),   Hetatom(Vector3(1, 1, 1), 1, "C", "C", 1)};
        vector<vector<Atom>> a = {b1, b2, b3};
        Protein protein(a, w);

        // set the weights to 1 so we can analytically determine the result
        // waters
        for (auto& atom : protein.hydration_atoms) {
            atom.set_effective_charge(1);
        }
        // atoms
        for (const auto& body : protein.bodies) {
            for (auto& atom : body.protein_atoms) {
                atom.set_effective_charge(1);
            }
        }
        protein.updated_charge = true;

        // calculate the histogram
        shared_ptr<ScatteringHistogram> hist = protein.get_histogram();
        const vector<double> d = hist->p_tot;

        // calculation: 8 identical points. 
        //      each point has:
        //          1 line  of length 0
        //          3 lines of length 2
        //          3 lines of length sqrt(2*2^2) = sqrt(8) = 2.82
        //          1 line  of length sqrt(3*2^2) = sqrt(12) = 3.46
        const vector<double> d_exp = {8, 0, 2*8*3, 8, 0, 0, 0, 0, 0, 0};
        for (size_t i = 0; i < d_exp.size(); i++) {
            if (d[i] != d_exp[i]) {
                cout << "Failed on index " << i << ". Values: " << d[i] << ", " << d_exp[i] << endl;
                REQUIRE(false);
            }
        }
    }

    SECTION("compare with body, simple") {
        // make the protein
        vector<Atom> b1 = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1)};
        vector<Atom> b2 = {Atom(Vector3(1, -1, -1), 1, "C", "C", 1), Atom(Vector3(1, 1, -1), 1, "C", "C", 1)};
        vector<Atom> b3 = {Atom(Vector3(-1, -1, 1), 1, "C", "C", 1), Atom(Vector3(-1, 1, 1), 1, "C", "C", 1)};
        vector<Atom> b4 = {Atom(Vector3(1, -1, 1), 1, "C", "C", 1), Atom(Vector3(1, 1, 1), 1, "C", "C", 1)};
        vector<vector<Atom>> ap = {b1, b2, b3, b4};
        Protein protein(ap, {});

        // make the body
        vector<Atom> ab = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1),
                            Atom(Vector3(1, -1, -1), 1, "C", "C", 1), Atom(Vector3(1, 1, -1), 1, "C", "C", 1),
                            Atom(Vector3(-1, -1, 1), 1, "C", "C", 1), Atom(Vector3(-1, 1, 1), 1, "C", "C", 1),
                            Atom(Vector3(1, -1, 1), 1, "C", "C", 1), Atom(Vector3(1, 1, 1), 1, "C", "C", 1)};
        Body body(ab, {});

        // create some water molecules
        vector<Hetatom> atoms(10);
        for (size_t i = 0; i < atoms.size(); i++) {
            atoms[i] = Hetatom::create_new_water(Vector3(i, i, i));
        }

        body.hydration_atoms = atoms;
        protein.hydration_atoms = atoms;

        // we now have a protein consisting of three bodies with the exact same contents as a single body.
        // the idea is now to compare the ScatteringHistogram output from their distance calculations, since it
        // is far easier to do for the single body. 
        shared_ptr<ScatteringHistogram> d_b = body.get_histogram();
        shared_ptr<ScatteringHistogram> d_p = protein.get_histogram();

        // direct access to the histogram data (only p_tot is defined)
        const vector<double>& p_tot = d_p->p_tot;
        const vector<double>& b_tot = d_b->p_tot;

        // compare each entry
        for (size_t i = 0; i < b_tot.size(); i++) {
            if (!approx(p_tot[i], b_tot[i])) {
                cout << "Failed on index " << i << ". Values: " << p_tot[i] << ", " << b_tot[i] << endl;
                REQUIRE(false);
            }
        }
    }

    SECTION("compare with body, real input") {
        Body body("data/2epe.pdb");
        body.center();
        
        // We iterate through the protein data from the body, and split it into multiple pieces of size 100.  
        vector<vector<Atom>> patoms; // vector containing the pieces we split it into
        vector<Atom> p_current(100); // vector containing the current piece
        size_t index = 0; // current index in p_current
        for (size_t i = 0; i < body.protein_atoms.size(); i++) {
            p_current[index] = body.protein_atoms[i];
            index++;
            if (index == 100) { // if index is 100, reset to 0
                patoms.push_back(p_current);
                index = 0;
            }
        }

        // add the final few atoms to our list
        if (index != 0) {
            p_current.resize(index);
            patoms.push_back(p_current);
        }

        // create the atom, and perform a sanity check on our extracted list
        Protein protein(patoms, {});
        vector<Atom> protein_atoms = protein.get_protein_atoms();
        vector<Atom> body_atoms = body.get_protein_atoms();

        // sizes must be equal. this also serves as a separate consistency check on the body generation. 
        if (protein_atoms.size() != body_atoms.size()) {
            cout << "Sizes " << protein_atoms.size() << " and " << body_atoms.size() << " should be equal. " << endl;
            REQUIRE(false);
        }

        // stronger consistency check - we check that all atoms are equal, and appear in the exact same order
        for (size_t i = 0; i < protein_atoms.size(); i++) {
            if (protein_atoms[i] != body_atoms[i]) {
                cout << "Comparison failed on index " << i << endl;
                cout << protein_atoms[i].as_pdb() << endl;
                cout << body_atoms[i].as_pdb() << endl;
                REQUIRE(false);
            }
        }

        // generate a hydration layer for the body, and copy it over to the protein
        body.generate_new_hydration();
        protein.hydration_atoms = body.hydration_atoms;

        // generate the distance histograms
        shared_ptr<ScatteringHistogram> d_b = body.get_histogram();
        shared_ptr<ScatteringHistogram> d_p = protein.get_histogram();

        // direct access to the histogram data (only p_tot is defined)
        const vector<double>& p_tot = d_p->p_tot;
        const vector<double>& b_tot = d_b->p_tot;

        // compare each entry
        for (size_t i = 0; i < b_tot.size(); i++) {
            if (!approx(p_tot[i], b_tot[i])) {
                cout << "Failed on index " << i << ". Values: " << p_tot[i] << ", " << b_tot[i] << endl;
                REQUIRE(false);
            }
        }

    }
}

TEST_CASE("get_cm", "[grid]") {
    // make the protein
    vector<Atom> b1 = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1)};
    vector<Atom> b2 = {Atom(Vector3(1, -1, -1), 1, "C", "C", 1), Atom(Vector3(1, 1, -1), 1, "C", "C", 1)};
    vector<Atom> b3 = {Atom(Vector3(-1, -1, 1), 1, "C", "C", 1), Atom(Vector3(-1, 1, 1), 1, "C", "C", 1)};
    vector<Atom> b4 = {Atom(Vector3(1, -1, 1), 1, "C", "C", 1), Atom(Vector3(1, 1, 1), 1, "C", "C", 1)};
    vector<vector<Atom>> ap = {b1, b2, b3, b4};
    Protein protein(ap, {});

    Vector3 cm = protein.get_cm();
    REQUIRE(cm == Vector3{0, 0, 0});
}

TEST_CASE("get_volume", "[grid]") {
    // make the protein
    vector<Atom> b1 = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1)};
    vector<Atom> b2 = {Atom(Vector3(1, -1, -1), 1, "C", "C", 1), Atom(Vector3(1, 1, -1), 1, "C", "C", 1)};
    vector<Atom> b3 = {Atom(Vector3(-1, -1, 1), 1, "C", "C", 1), Atom(Vector3(-1, 1, 1), 1, "C", "C", 1)};
    vector<Atom> b4 = {Atom(Vector3(1, -1, 1), 1, "C", "C", 1), Atom(Vector3(1, 1, 1), 1, "C", "C", 1)};
    vector<vector<Atom>> ap = {b1, b2, b3, b4};
    Protein protein(ap, {});

    REQUIRE(protein.get_volume_acids() == Approx(3*constants::volume::lysine));

}

/**
 * @brief Test that the grid is exactly identical when generated by a single body as when generated by a collection of bodies from a protein.
 */
TEST_CASE("compare grid placement", "[grid]") {
    vector<Atom> a = {Atom(Vector3(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3(-1, 1, -1), 1, "C", "C", 1), 
                      Atom(Vector3(1, -1, -1), 1, "C", "C", 1),  Atom(Vector3(1, 1, -1), 1, "C", "C", 1), 
                      Atom(Vector3(-1, -1, 1), 1, "C", "C", 1),  Atom(Vector3(-1, 1, 1), 1, "C", "C", 1),
                      Atom(Vector3(1, -1, 1), 1, "C", "C", 1),   Atom(Vector3(1, 1, 1), 1, "C", "C", 1)};
    vector<Hetatom> w = {};
    
    Protein protein(a, w);
    Body body(a, w);

    REQUIRE(protein.get_volume_grid() == Approx(body.get_volume_grid()));
    shared_ptr<Grid> gp = protein.get_grid();
    shared_ptr<Grid> gb = body.get_grid();

    vector<vector<vector<char>>>& grid_protein = gp->grid;
    vector<vector<vector<char>>>& grid_body = gb->grid;

    vector<vector<int>> bounds = gp->bounding_box();
    for (int i = bounds[0][0]-10; i < bounds[0][1]+10; i++) {
        for (int j = bounds[1][0]-10; j < bounds[1][1]+10; j++) {
            for (int k = bounds[2][0]-10; k < bounds[2][1]+10; k++) {
                if (grid_protein[i][j][k] != grid_body[i][j][k]) {
                    cout << "Test failed. Expected " << grid_body[i][j][k] << ", received " << grid_protein[i][j][k] << endl;
                    REQUIRE(false);
                }
            }
        }
    }
}