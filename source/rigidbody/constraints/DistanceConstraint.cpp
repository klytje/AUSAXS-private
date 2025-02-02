/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <rigidbody/constraints/DistanceConstraint.h>
#include <settings/RigidBodySettings.h>
#include <constants/Constants.h>
#include <data/Molecule.h>
#include <data/Body.h>
#include <data/record/Atom.h>

using namespace rigidbody::constraints;
using namespace data;
using namespace data::record;

DistanceConstraint::DistanceConstraint(data::Molecule* protein, unsigned int ibody1, unsigned int ibody2, unsigned int iatom1, unsigned int iatom2) 
    : protein(protein), ibody1(ibody1), ibody2(ibody2), iatom1(iatom1), iatom2(iatom2) {
    const Body& body1 = protein->get_body(ibody1);
    const Body& body2 = protein->get_body(ibody2);
    const Atom& atom1 = body1.get_atom(iatom1);
    const Atom& atom2 = body2.get_atom(iatom2);

    // we only want to allow constraints between the backbone C-alpha structure
    if (atom1.element != constants::atom_t::C || atom2.element != constants::atom_t::C) {
        throw except::invalid_argument("Constraint::Constraint: Constraints only makes sense between the carbon-atoms of the backbone!");
    }

    // constraints within the same body doesn't make sense
    if (body1.get_id() == body2.get_id()) {throw except::invalid_argument("Constraint::Constraint: Cannot create a constraint between atoms in the same body!");}

    // set the base radius and perform a sanity check
    r_base = atom1.distance(atom2);
    if (r_base > 4) {throw except::invalid_argument(
        "Constraint::Constraint: The atoms being constrained are too far apart!\n"
        "Atom 1: " + atom1.name + " in body " + std::to_string(ibody1) + " at " + atom1.get_coordinates().to_string() + "\n"
        "Atom 2: " + atom2.name + " in body " + std::to_string(ibody2) + " at " + atom2.get_coordinates().to_string() + "\n"
    );}
}

DistanceConstraint::DistanceConstraint(data::Molecule* protein, const Atom& atom1, const Atom& atom2) : protein(protein) {
    // we only want to allow constraints between the backbone C-alpha structure
    if (atom1.element != constants::atom_t::C || atom2.element != constants::atom_t::C) {
        throw except::invalid_argument("Constraint::Constraint: Constraints only makes sense between the carbon-atoms of the backbone!");
    }

    auto[loc1, loc2] = find_host_bodies(atom1, atom2);

    ibody1 = loc1.body;
    ibody2 = loc2.body;
    iatom1 = loc1.atom;
    iatom2 = loc2.atom;

    // constraints within the same body doesn't make sense
    if (ibody1 == ibody2) {throw except::invalid_argument("Constraint::Constraint: Cannot create a constraint between atoms in the same body!");}

    // set the base radius and perform a sanity check
    r_base = atom1.distance(atom2);
    if (r_base > settings::rigidbody::bond_distance) {throw except::invalid_argument("Constraint::Constraint: The atoms being constrained are too far apart!");}
}

std::pair<DistanceConstraint::AtomLoc, DistanceConstraint::AtomLoc> DistanceConstraint::find_host_bodies(const Atom& atom1, const Atom& atom2) const {
    int ibody1 = -1, ibody2 = -1;
    int iatom1 = -1, iatom2 = -1;
    for (unsigned int ibody = 0; ibody < protein->body_size(); ibody++) {
        const Body& body = protein->get_body(ibody);
        for (unsigned int iatom = 0; iatom < body.get_atoms().size(); iatom++) {
            if (atom1 == body.get_atom(iatom)) {
                ibody1 = ibody;
                iatom1 = iatom;
                break; // a1 and a2 *must* be from different bodies, so we break
            } else if (atom2 == body.get_atom(iatom)) {
                ibody2 = ibody;
                iatom2 = iatom;
                break; // same
            }
        }
    }

    // check that both b1 and b2 were found
    if (ibody1 == -1 || ibody2 == -1) {
        throw except::invalid_argument("RigidBody::create_constraint: Could not determine host bodies for the two atoms.");
    }

    return std::make_pair(AtomLoc{.body=ibody1, .atom=iatom1}, AtomLoc{.body=ibody2, .atom=iatom2});
}

double DistanceConstraint::evaluate() const {
    const Atom& atom1 = protein->get_body(ibody1).get_atom(iatom1);
    const Atom& atom2 = protein->get_body(ibody2).get_atom(iatom2);
    return transform(r_base - atom1.distance(atom2));
}

bool DistanceConstraint::operator==(const DistanceConstraint& constraint) const {
    return ibody1 == constraint.ibody1 
        && ibody2 == constraint.ibody2 
        && iatom1 == constraint.iatom1 
        && iatom2 == constraint.iatom2;
}

double DistanceConstraint::transform(double offset) {
    return offset*offset*offset*offset*10;
}

const Atom& DistanceConstraint::get_atom1() const {
    return protein->get_body(ibody1).get_atom(iatom1);
}

const Atom& DistanceConstraint::get_atom2() const {
    return protein->get_body(ibody2).get_atom(iatom2);
}

const Body& DistanceConstraint::get_body1() const {
    return protein->get_body(ibody1);
}

const Body& DistanceConstraint::get_body2() const {
    return protein->get_body(ibody2);
}

Body& DistanceConstraint::get_body1() {
    return protein->get_body(ibody1);
}

Body& DistanceConstraint::get_body2() {
    return protein->get_body(ibody2);
}

std::string DistanceConstraint::to_string() const {
    std::stringstream ss;
    ss << "Constraint between (" << protein->get_body(ibody1).get_atom(iatom1).serial << ", " << protein->get_body(ibody1).get_atom(iatom1).name << ") and "
                             "(" << protein->get_body(ibody2).get_atom(iatom2).serial << ", " << protein->get_body(ibody2).get_atom(iatom2).name << ")";
    return ss.str();
}