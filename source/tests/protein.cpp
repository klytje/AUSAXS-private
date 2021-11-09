// includes
#include <vector>
#include <string>
#include <fstream>

#include "tests/Test.h"
#include "Protein.cpp"

using namespace ROOT;

void create_test_file() {
    std::ofstream file("temp.pdb");
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
}

void test_get_cm() {
    Protein protein("temp.pdb");

    TVector3 cm = protein.get_cm();
    IS_TRUE(cm[0] == 0);
    IS_TRUE(cm[1] == 0);
    IS_TRUE(cm[2] == 0);
}

void test_generate_grid() {
    Protein protein("temp.pdb");

    double width = 0.1;
    auto[corner, bins] = protein.generate_grid(width);
    IS_TRUE(corner[0] == -1);
    IS_TRUE(corner[1] == -1);
    IS_TRUE(corner[2] == -1);
    IS_TRUE(bins[0] == 2/width);
}

int main(void)
{
    cout << "Summary of Protein testing:" << endl;
    create_test_file();
    test_get_cm();
    test_generate_grid();
    remove("temp.pdb");

    cout << "\033[1;32m" << "All protein tests passed." << "\033[0m" << endl;
    return 0;
}