// includes
#include <vector>
#include <string>
#include <fstream>

#include "tests/Test.h"
#include "data/Protein.h"
#include "hydrate/Grid.h"
#include "constants.h"

void create_test_file() {
    std::ofstream file1("temp1.pdb");
    std::ofstream file2("temp2.pdb");
    std::ofstream file3("temp3.pdb");
    // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
    file1 << "ATOM      1  C   LYS A   1          -1      -1      -1  1.00 00.00           C \n"
          << "ATOM      2  C   LYS A   1          -1       1      -1  1.00 00.00           C \n"
          << "ATOM      3  C   LYS A   1           1      -1      -1  1.00 00.00           C \n"
          << "ATOM      4  C   LYS A   1           1       1      -1  1.00 00.00           C \n";
    file1.close();
    file2 << "ATOM      5  C   LYS A   1          -1      -1       1  1.00 00.00           C \n"
          << "ATOM      6  C   LYS A   1          -1       1       1  1.00 00.00           C \n"
          << "ATOM      7  C   LYS A   1           1      -1       1  1.00 00.00           C \n"
          << "ATOM      8  C   LYS A   1           1       1       1  1.00 00.00           C \n";
    file2.close();
    file3 << "ATOM      9  C   LYS A   1           0       0       0  1.00 00.00           C  ";
    file3.close();
}

void test_calc_distances() {
    std::ofstream file("temp_dist.pdb");
    // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
    file << "ATOM      1  C   LYS A   1          -1      -1      -1  1.00 00.00           C \n"
         << "ATOM      2  C   LYS A   1          -1       1      -1  1.00 00.00           C \n"
         << "ATOM      3  C   LYS A   1           1      -1      -1  1.00 00.00           C \n"
         << "ATOM      4  C   LYS A   1           1       1      -1  1.00 00.00           C \n"

         << "ATOM      5  C   LYS A   1          -1      -1       1  1.00 00.00           C \n"
         << "ATOM      6  C   LYS A   1          -1       1       1  1.00 00.00           C \n"
         << "ATOM      7  C   LYS A   1           1      -1       1  1.00 00.00           C \n"
         << "ATOM      8  C   LYS A   1           1       1       1  1.00 00.00           C \n"

         << "ATOM      9  C   LYS A   1           0       0       0  1.00 00.00           C  ";
    file.close();

    // create some water molecules
    vector<Hetatom> atoms(10);
    for (size_t i = 0; i < atoms.size(); i++) {
        atoms[i] = Hetatom::create_new_water(Vector3(i, i, i));
    }

    // create a dummy signalling object for the body
    StateManager sm_dummy(1);
    std::shared_ptr<StateManager::Signaller> dummy = sm_dummy.get_probe(0);

    Protein protein({"temp1.pdb", "temp2.pdb", "temp3.pdb"});
    Body body("temp_dist.pdb", dummy);
    remove("temp_dist.pdb");

    body.hydration_atoms = atoms;
    protein.hydration_atoms = atoms;

    // we now have a protein consisting of three bodies with the exact same contents as a single body.
    // the idea is now to compare the ScatteringHistogram output from their distance calculations, since it
    // is far easier to do for the single body. 
    shared_ptr<ScatteringHistogram> d_b = body.get_histogram();
    shared_ptr<ScatteringHistogram> d_p = protein.get_histogram();

    // direct access to the histogram data
    const vector<double>& p_pp = d_p->p_pp;
    const vector<double>& p_hh = d_p->p_hh;
    const vector<double>& p_hp = d_p->p_hp;
    const vector<double>& p_tot = d_p->p_tot;

    const vector<double>& b_pp = d_b->p_pp;
    const vector<double>& b_hh = d_b->p_hh;
    const vector<double>& b_hp = d_b->p_hp;
    const vector<double>& b_tot = d_b->p_tot;

    // compare each entry
    for (size_t i = 0; i < d_b->q.size(); i++) {
        if (p_pp[i] != b_pp[i]) {
            IS_TRUE(false);
            break;
        }
        if (p_hh[i] != b_hh[i]) {
            IS_TRUE(false);
            break;
        }
        if (p_hp[i] != b_hp[i]) {
            IS_TRUE(false);
            break;
        }
        if (p_tot[i] != b_tot[i]) {
            IS_TRUE(false);
            break;
        }
    }
}

void test_get_cm() {
    Protein protein({"temp1.pdb", "temp2.pdb", "temp3.pdb"});
    Vector3 cm = protein.get_cm();
    IS_TRUE(cm == Vector3({0, 0, 0}));
}

void test_volume() {
    Protein protein({"temp1.pdb", "temp2.pdb", "temp3.pdb"});
    IS_TRUE(protein.get_volume_acids() == 3*constants::volume::lysine);
}

int main(void) {
    cout << "Summary of Protein testing:" << endl;
    create_test_file();
    test_get_cm();
    test_volume();
    test_calc_distances();
    remove("temp1.pdb");
    remove("temp2.pdb");
    remove("temp3.pdb");

    if (passed_all) {
        cout << "\033[1;32m" << "All Protein tests passed." << "\033[0m" << endl;
    } else {
        print_err("Some Protein tests failed.");
    }
    return 0;
}