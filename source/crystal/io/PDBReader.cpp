#include <crystal/io/PDBReader.h>
#include <data/Protein.h>
#include <utility/Axis3D.h>
#include <utility/Basis3D.h>
#include <settings/CrystalSettings.h>
#include <settings/HistogramSettings.h>
#include <io/ExistingFile.h>
#include <data/Atom.h>
#include <data/Water.h>
#include <data/Body.h>

std::pair<Basis3D, std::vector<Vector3<double>>> crystal::io::PDBReader::read(const ::io::ExistingFile& input) const {
    Protein protein(input);
    double expansion = settings::crystal::grid_expansion;

    auto prot_atoms = protein.get_atoms();
    if (prot_atoms.empty()) {throw except::invalid_argument("PDBReader::read: No atoms were found in file \"" + input + "\".");}
    auto position = prot_atoms[0].get_coordinates();
    Axis3D axis({position.x(), position.x(), position.y(), position.y(), position.z(), position.z()});
    for (const auto& atom : prot_atoms) {
        auto position = atom.get_coordinates();
        if (position.x() < axis.x.min) {axis.x.min = position.x();}
        if (position.x() > axis.x.max) {axis.x.max = position.x();}
        if (position.y() < axis.y.min) {axis.y.min = position.y();}
        if (position.y() > axis.y.max) {axis.y.max = position.y();}
        if (position.z() < axis.z.min) {axis.z.min = position.z();}
        if (position.z() > axis.z.max) {axis.z.max = position.z();}
    }

    // quick check to see if the box is too large
    double distance = std::sqrt(std::pow(axis.x.span(), 2) + std::pow(axis.y.span(), 2) + std::pow(axis.z.span(), 2));
    if (distance > settings::axes::max_distance) {
        throw except::io_error("PDBReader::read: The distance between the edges of the box is " + std::to_string(distance) + " Å, which is larger than the maximum allowed distance of " + std::to_string(settings::axes::max_distance) + " Å.");
    }

    // center the protein in the middle of the box
    double factor = (expansion-1)/2;
    protein.translate({-axis.x.min + factor*axis.x.span(), -axis.y.min + factor*axis.y.span(), -axis.z.min + factor*axis.z.span()});
    std::vector<Vector3<double>> atoms;
    for (const auto& atom : prot_atoms) {
        atoms.push_back(atom.get_coordinates());
    }

    // expand the box by 2 times
    axis.x.min -= axis.x.span()*expansion;
    axis.x.max += axis.x.span()*expansion;
    axis.y.min -= axis.y.span()*expansion;
    axis.y.max += axis.y.span()*expansion;
    axis.z.min -= axis.z.span()*expansion;
    axis.z.max += axis.z.span()*expansion;

    Basis3D basis({axis.x.span(), 0, 0}, {0, axis.y.span(), 0}, {0, 0, axis.z.span()});
    return std::make_pair(std::move(basis), std::move(atoms));
}