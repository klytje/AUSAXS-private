#pragma once

// includes
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

// my own includes
#include "Reader.h"

class PDB_reader : public Reader {
public: 
    /** Constructor for the PDB_reader class. 
     * @param filename the name of the input file
     */
    PDB_reader(string filename) : Reader(filename) {
        if (filename.find(".pdb") == string::npos) {
            print_err("Input file \"" + filename + "\" is not a .xml file!");
            exit(1);
        }
    };

    /** Parse the atoms from the input file
     * @return A vector containing all of the parsed atoms.
     */
    vector<Atom*> read() override {
        string line;
        Atom* atom = new Atom();
        vector<Atom*> atoms;
        int serial = 1;
        atom->set_serial(serial);

        while(getline(file, line)) {
            /* All lines are of the form
            * [ATOM   9999  N   LYS A 999      -3.462  69.119  -8.662  1.00 19.81          Ne ] (sample line)
            * [0      7     13  17  21         31      39      47      55   61           75   ] (index numbers)
            * so the task here is simply to access the correct indices and convert it to the proper data format
            */
        
            // check if it is an ATOM or HETATM line (chars 0 - 6)
            string desc = line.substr(0, 6);
            if (!(desc == "ATOM  " || desc == "HETATM")) {
                if (desc == "TER   ") { // not sure what this means, but it increments the serial for some reason
                    serial++;
                }
                continue; // otherwise we just skip it
            }

            // extract the serial number (chars 7 - 12)
            int n = 0; // number of digits
            for (int i = 7; i < 11; i++) {
                if (std::isdigit(line[i])) {
                    n++;
                }
            }
            int this_serial = atoi(line.substr(11-n, n).c_str());
            // sanity check on the serial number
            if (this_serial != serial) { // the read serial should be previous serial + 1
                print_err((format("ERROR: Broken reading sequence in file %1%. Expected id %2%, but found %3%.") % get_filename() % serial % this_serial).str());
                exit(1);
            }

            // extract the chemical composition (chars 13 - 16)
            string comp = line.substr(17, 4);
            boost::erase_all(comp, " "); // remove any spaces
            atom->set_comp(comp);

            // extract the coordinates (chars 31 - 38, 39 - 46, 47 - 54)
            string x = line.substr(31, 7);
            string y = line.substr(39, 7);
            string z = line.substr(47, 7);
            TVector3 coords(stod(x), stod(y), stod(z));
            atom->set_coordinates(coords);

            // extract the occupancy (chars 55 - 60)
            string occupancy = line.substr(55, 5);
            atom->set_occupancy(stod(occupancy));

            // extract the atomic symbol (chars 75 - 78)
            string symbol = line.substr(75, 3);
            boost::erase_all(symbol, " "); // remove any spaces
            atom->set_symbol(symbol);

            // create the atom and prepare the next one
            atoms.push_back(atom);
            atom = new Atom();

            serial++;
            atom->set_serial(serial);
        }
        return atoms;
    }
};