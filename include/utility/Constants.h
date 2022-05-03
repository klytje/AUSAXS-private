#pragma once

#include <map>
#include <string>

using std::string;

constexpr double simple_pow(double val, unsigned int power) {
    double sum = 1;
    for (unsigned int i = 0; i < power; i++) {
        sum *= val;
    }
    return sum;
}

/**
 * @brief \namespace constants
 * 
 * This namespace contains all constants used in this project. 
 */
namespace constants {
    /**
     * @brief \namespace radius
     * 
     * This namespace contains all the radius constants used in this project. 
     */
    namespace radius {
        constexpr double electron = 0.0000281794; // electron radius in units of Ångström
    }
    constexpr double Avogadro = 6.02214076e-23; // mol^-1

    /**
     * @brief \namespace unit
     * 
     * This namespace contains all the unit conversion constants used in this project. 
     */
    namespace unit { // units with respect to the ones used in this file
        constexpr double mg = 1.66054e-21; // Dalton --> mg
        constexpr double gm = 1.66054e-24; // Dalton --> grams
        constexpr double cm = 1e-8; // Ångström --> cm

        constexpr double mL = simple_pow(unit::cm, 3); // Ångström^3 --> mL
    }

    // The 1-symbol names of all amino acids. 
    const std::map<string, char> name_1symbol_map = {{"glycine", 'G'}, {"alanine", 'A'}, {"valine", 'V'}, {"leucine", 'L'}, {"isoleucine", 'I'}, 
        {"phenylalanine", 'F'}, {"tyrosine", 'Y'}, {"tryptophan", 'W'}, {"aspartic_acid", 'D'}, {"glutamic_acid", 'E'}, {"serine", 'S'}, 
        {"threonine", 'T'}, {"asparagine", 'N'}, {"glutamine", 'Q'}, {"lysine", 'K'}, {"arginine", 'R'}, {"histidine", 'H'}, {"methionine", 'M'}, 
        {"cysteine", 'C'}, {"proline", 'P'}
    };

    // The 3-symbol names of all amino acids. 
    const std::map<string, string> name_3symbol_map = {{"glycine", "GLY"}, {"alanine", "ALA"}, {"valine", "VAL"}, {"leucine", "LEU"}, 
        {"isoleucine", "ILE"}, {"phenylalanine", "PHE"}, {"tyrosine", "TYR"}, {"tryptophan", "TRP"}, {"aspartic_acid", "ASP"}, {"glutamic_acid", "GLU"}, 
        {"serine", "SER"}, {"threonine", "THR"}, {"asparagine", "ASN"}, {"glutamine", "GLN"}, {"lysine", "LYS"}, {"arginine", "ARG"}, {"histidine", "HIS"}, 
        {"methionine", "MET"}, {"cysteine", "CYS"}, {"proline", "PRO"}
    };


    /**
     * @brief \namespace volume
     * 
     * This namespace contains the volume of all amino acids. 
     * They are taken from https://doi.org/10.1088/0034-4885/39/10/001.
     * All values are in Å^3
     */
    namespace volume {
        constexpr double glycine = 66.4;
        constexpr double alanine = 91.5;
        constexpr double valine = 141.7;
        constexpr double leucine = 167.9;
        constexpr double isoleucine = 168.8;
        constexpr double phenylalanine = 203.5;
        constexpr double tyrosine = 203.6;
        constexpr double tryptophan = 237.6;
        constexpr double aspartic_acid = 113.6;
        constexpr double glutamic_acid = 140.6;
        constexpr double serine = 99.1;
        constexpr double threonine = 122.1;
        constexpr double asparagine = 135.2;
        constexpr double glutamine = 161.1;
        constexpr double lysine = 176.2;
        constexpr double arginine = 180.8;
        constexpr double histidine = 167.3;
        constexpr double methionine = 170.8;
        constexpr double cysteine = 105.6;
        constexpr double proline = 129.3;

        // get the volume of a 3symbol amino acid
        const std::map<string, double> get = {{"GLY", glycine}, {"ALA", alanine}, {"VAL", valine}, {"LEU", leucine}, {"ILE", isoleucine}, 
            {"PHE", phenylalanine}, {"TYR", tyrosine}, {"TRP", tryptophan}, {"ASP", aspartic_acid}, {"GLU", glutamic_acid}, {"SER", serine}, 
            {"THR", threonine}, {"ASN", asparagine}, {"GLN", glutamine}, {"LYS", lysine}, {"ARG", arginine}, {"HIS", histidine}, 
            {"MET", methionine}, {"CYS", cysteine}, {"PRO", proline},
        };
    }

    /**
     * @brief \namespace mass 
     * 
     * This namespace contains the masses of the most common atomic elements encountered in SAXS. 
     * The weights are taken from https://www.chem.ualberta.ca/~massspec/atomic_mass_abund.pdf. 
     */
    namespace mass {
        constexpr double H = 1.008;  // Hydrogen mass
        constexpr double He = 4.003; // Helium mass
        constexpr double Li = 7.016; // Lithium mass
        constexpr double C = 12.011; // Carbon mass
        constexpr double N = 14.003; // Nitrogen mass
        constexpr double O = 15.995; // Oxygen mass
        constexpr double S = 31.972; // Sulphur mass

        // get the weight of an atom
        const std::map<string, double> atomic = {{"H", H}, {"He", He}, {"Li", Li}, {"C", C}, {"N", N}, {"O", O}, {"S", S}};
    }

    /**
     * @brief \namespace charge
     * 
     * This namespace contains the net charge of the most common atomic elements encountered in SAXS. 
     */
    namespace charge {
        constexpr int e = 1;  // Electron charge
        constexpr int H = 1;  // Hydrogen charge
        constexpr int He = 2; // Helium charge
        constexpr int Li = 3; // Lithium charge 
        constexpr int C = 6;  // Carbon charge
        constexpr int N = 7;  // Nitrogen charge
        constexpr int O = 8;  // Oxygen charge
        constexpr int S = 16; // Sulphur charge

        // get the charge Z of an atom
        const std::map<string, int> get = {{"H", H}, {"He", He}, {"Li", Li}, {"C", C}, {"N", N}, {"O", O}, {"S", S}};

        namespace density {
            constexpr double water = 0.334; // e/Å^3
        }
    }

    /**
     * @brief \namespace hydrogen_atoms
     * 
     * This namespace contains the number of hydrogen atoms attached to all amino acids. 
     */
    namespace hydrogen_atoms {
        namespace none {
            const std::map<string, int> get = {{"", 0}};
        }
        namespace water {
            constexpr int O = 2;
            const std::map<string, int> get = {{"O", O}};
        }
        namespace glycine {
            constexpr int N = 1;
            constexpr int CA = 2;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}};
        }
        namespace alanine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 3;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}};
        }
        namespace valine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 1;
            constexpr int CG1 = 3;
            constexpr int CG2 = 3;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG1", CG1}, {"CG2", CG2}};
        }
        namespace leucine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 1;
            constexpr int CD1 = 3;
            constexpr int CD2 = 3;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD1", CD1}, 
                {"CD2", CD2}};
        }
        namespace isoleucine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 1;
            constexpr int CG2 = 3;
            constexpr int CG1 = 2;
            constexpr int CD1 = 3;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG2", CG2}, {"CG1", CG1}, 
                {"CD1", CD1}};
        }
        namespace phenylalanine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 0;
            constexpr int CD1 = 1;
            constexpr int CD2 = 1;
            constexpr int CE1 = 1;
            constexpr int CE2 = 1;
            constexpr int CZ = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD1", CD1}, 
                {"CD2", CD2}, {"CE1", CD1}, {"CE2", CD2}, {"CZ", CZ}};
        }
        namespace tyrosine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 0;
            constexpr int CD1 = 1;
            constexpr int CD2 = 1;
            constexpr int CE1 = 1;
            constexpr int CE2 = 1;
            constexpr int CZ = 0;
            constexpr int OH = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD1", CD1}, 
                {"CD2", CD2}, {"CE1", CE1}, {"CE2", CE2}, {"CZ", CZ}, {"OH", OH}};
        }
        namespace tryptophan {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 0;
            constexpr int CD1 = 1;
            constexpr int CD2 = 0;
            constexpr int NE1 = 1;
            constexpr int CE2 = 0;
            constexpr int CE3 = 1;
            constexpr int CZ2 = 1;
            constexpr int CZ3 = 1;
            constexpr int CH2 = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD1", CD1}, 
                {"CD2", CD2}, {"NE1", NE1}, {"CE2", CE2}, {"CE3", CE3}, {"CZ2", CZ2}, {"CZ3", CZ3}, {"CH2", CH2}};
        }
        namespace aspartic_acid {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 0;
            constexpr int OD1 = 0;
            constexpr int OD2 = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"OD1", OD1}, 
                {"OD2", OD2}};
        }
        namespace glutamic_acid {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 2;
            constexpr int CD = 0;
            constexpr int OE1 = 0;
            constexpr int OE2 = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD", CD}, 
                {"OE1", OE1}, {"OE2", OE2}};
        }
        namespace serine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int OG = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"OG", OG}};
        }
        namespace threonine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 1;
            constexpr int OG1 = 1;
            constexpr int CG2 = 3;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"OG1", OG1}, {"CG2", CG2}};
        }
        namespace asparagine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 0;
            constexpr int OD1 = 0;
            constexpr int ND2 = 2;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"OD1", OD1}, 
                {"ND2", ND2}};
        }
        namespace glutamine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 2;
            constexpr int CD = 0;
            constexpr int OE1 = 0;
            constexpr int NE2 = 2;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD", CD}, 
                {"OE1", OE1}, {"NE2", NE2}};
        }
        namespace lysine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 2;
            constexpr int CD = 2;
            constexpr int CE = 2;
            constexpr int NZ = 2;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD", CD}, 
                {"CE", CE}, {"NZ", NZ}};
        }
        namespace arginine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 2;
            constexpr int CD = 2;
            constexpr int NE = 1;
            constexpr int CZ = 0;
            constexpr int NH1 = 1;
            constexpr int NH2 = 2;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD", CD}, 
                {"NE", NE}, {"CZ", CZ}, {"NH1", NH1}, {"NH2", NH2}};
        }
        namespace histidine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 0;
            constexpr int ND1 = 1;
            constexpr int CD2 = 1;
            constexpr int CE1 = 1;
            constexpr int NE2 = 0;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"ND1", ND1}, 
                {"CD2", CD2}, {"CE1", CE1}, {"NE2", NE2}};
        }
        namespace methionine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 2;
            constexpr int SD = 0;
            constexpr int CE = 3;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"SD", SD}, 
                {"CE", CE}};
        }
        namespace cysteine {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int SG = 1;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"SG", SG}};
        }
        namespace proline {
            constexpr int N = 1;
            constexpr int CA = 1;
            constexpr int C = 0;
            constexpr int O = 0;
            constexpr int OXT = 1;
            constexpr int CB = 2;
            constexpr int CG = 2;
            constexpr int CD = 2;
            const std::map<string, int> get = {{"N", N}, {"CA", CA}, {"C", C}, {"O", O}, {"OXT", OXT}, {"CB", CB}, {"CG", CG}, {"CD", CD}};
        }
        namespace myristic_acid {
            constexpr int C1 = 0;
            constexpr int O1 = 0;
            constexpr int O2 = 1;
            constexpr int C2 = 2;
            constexpr int C3 = 2;
            constexpr int C4 = 2;
            constexpr int C5 = 2;
            constexpr int C6 = 2;
            constexpr int C7 = 2;
            constexpr int C8 = 2;
            constexpr int C9 = 2;
            constexpr int C10 = 2;
            constexpr int C11 = 2;
            constexpr int C12 = 2;
            constexpr int C13 = 2;
            constexpr int C14 = 3;
            const std::map<string, int> get = {{"C1", C1}, {"O1", O1}, {"O2", O2}, {"C2", C2}, {"C3", C3}, {"C4", C4}, {"C5", C5}, {"C6", C6}, {"C7", C7},
                {"C8", C8}, {"C9", C9}, {"C10", C10}, {"C11", C11}, {"C12", C12}, {"C13", C13}, {"C14", C14}};
        }

        // get the number of hydrogen atoms attached to an atom of a specific acid. Example: get.at("GLY").at("CA") = 2
        const std::map<string, std::map<string, int>> get = {{"GLY", glycine::get}, {"ALA", alanine::get}, {"VAL", valine::get}, 
            {"LEU", leucine::get}, {"ILE", isoleucine::get}, {"PHE", phenylalanine::get}, {"TYR", tyrosine::get}, {"TRP", tryptophan::get}, 
            {"ASP", aspartic_acid::get}, {"GLU", glutamic_acid::get}, {"SER", serine::get}, {"THR", threonine::get}, {"ASN", asparagine::get}, 
            {"GLN", glutamine::get}, {"LYS", lysine::get}, {"ARG", arginine::get}, {"HIS", histidine::get}, {"MET", methionine::get}, 
            {"CYS", cysteine::get}, {"PRO", proline::get}, {"HOH", water::get}, {"MYR", myristic_acid::get}, {"", none::get}};
    }
}