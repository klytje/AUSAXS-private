#include <catch2/catch_test_macros.hpp>

#include <data/Molecule.h>
#include <data/record/Atom.h>
#include <data/record/Water.h>
#include <data/Body.h>
#include <data/state/StateManager.h>
#include <data/state/Signaller.h>
#include <hist/distance_calculator/HistogramManager.h>
#include <hist/intensity_calculator/ICompositeDistanceHistogram.h>
#include <hist/distance_calculator/HistogramManagerMT.h>
#include <hist/distance_calculator/HistogramManagerMTFFAvg.h>
#include <hist/distance_calculator/HistogramManagerMTFFExplicit.h>
#include <hist/distance_calculator/HistogramManagerMTFFGrid.h>
#include <hist/distance_calculator/PartialHistogramManager.h>
#include <hist/distance_calculator/PartialHistogramManagerMT.h>
#include <hist/intensity_calculator/CompositeDistanceHistogramFFAvg.h>
#include <form_factor/FormFactor.h>
#include <io/ExistingFile.h>
#include <settings/MoleculeSettings.h>
#include <settings/HistogramSettings.h>
#include <constants/Constants.h>

using namespace data::record;
using namespace data;

struct analytical_histogram {
    static void set_unity_charge(Molecule& protein) {
        // set the weights to 1 so we can analytically determine the result
        // waters
        for (auto& atom : protein.get_waters()) {
            atom.set_effective_charge(1);
        }
        // atoms
        for (auto& body : protein.get_bodies()) {
            for (auto& atom : body.get_atoms()) {
                atom.set_effective_charge(1);
            }
        }
    }

    // calculation: 8 identical points. 
    //      each point has:
    //          1 line  of length 0
    //          3 lines of length 2
    //          3 lines of length sqrt(2*2^2) = sqrt(8) = 2.82
    //          1 line  of length sqrt(3*2^2) = sqrt(12) = 3.46
    std::vector<double> calc_exp() {
        auto width = constants::axes::d_axis.width();
        std::vector<double> res(std::round(3.5/width)+1);
        res[0] = 8;
        res[std::round(2/width)] += 8*3;
        res[std::round(std::sqrt(8)/width)] += 8*3;
        res[std::round(std::sqrt(12)/width)] += 8*1;
        return res;
    }

    std::vector<double> p_exp = calc_exp();
};

/**
 * @brief Compare two histograms. 
 *        Only indices [0, p1.size()] are checked.
 */
bool compare_hist(Vector<double> p1, Vector<double> p2) {
    if (p2.size() < p1.size()) {
        std::cout << "Failed: p2.size() < p1.size() (" << p2.size() << " < " << p1.size() << ")" << std::endl;
        return false;
    }
    for (unsigned int i = 0; i < p1.size(); i++) {
        if (!utility::approx(p1[i], p2[i])) {
            std::cout << "Failed on index " << i << ". Values: " << p1[i] << ", " << p2[i] << std::endl;
            return false;
        }
    }
    return true;
}

TEST_CASE_METHOD(analytical_histogram, "HistogramManager::calculate_all") {
    settings::molecule::implicit_hydrogens = false;
    settings::molecule::use_effective_charge = false;
    settings::general::verbose = false;
    settings::hist::histogram_manager = settings::hist::HistogramManagerChoice::HistogramManager;

    SECTION("analytical") {
        SECTION("atoms only") {
            // the following just describes the eight corners of a cube centered at origo
            std::vector<Atom> b1 = {Atom(Vector3<double>(-1, -1, -1), 1, constants::atom_t::C, "C", 1), Atom(Vector3<double>(-1, 1, -1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Atom> b2 = {Atom(Vector3<double>( 1, -1, -1), 1, constants::atom_t::C, "C", 1), Atom(Vector3<double>( 1, 1, -1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Atom> b3 = {Atom(Vector3<double>(-1, -1,  1), 1, constants::atom_t::C, "C", 1), Atom(Vector3<double>(-1, 1,  1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Atom> b4 = {Atom(Vector3<double>( 1, -1,  1), 1, constants::atom_t::C, "C", 1), Atom(Vector3<double>( 1, 1,  1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Body> a = {Body(b1), Body(b2), Body(b3), Body(b4)};
            Molecule protein(a);
            set_unity_charge(protein);

            { // hm
                auto hm = hist::HistogramManager<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm->get_total_counts()));

                hm = hist::HistogramManager<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm->get_total_counts()));
            }
            { // hm_mt
                auto hm_mt = hist::HistogramManagerMT<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt->get_total_counts()));

                hm_mt = hist::HistogramManagerMT<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt->get_total_counts()));
            }
            { // hm_mt_ff_avg
                auto hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_avg->get_total_counts()));

                hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_avg->get_total_counts()));
            }
            { // hm_mt_ff_explicit
                auto hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_explicit->get_total_counts()));

                hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_explicit->get_total_counts()));
            }
            { // hm_mt_ff_grid
                auto hm_mt_ff_grid = hist::HistogramManagerMTFFGrid<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_grid->get_total_counts()));

                hm_mt_ff_grid = hist::HistogramManagerMTFFGrid<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_grid->get_total_counts()));
            }
            { // phm
                auto phm = hist::PartialHistogramManager<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm->get_total_counts()));

                phm = hist::PartialHistogramManager<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm->get_total_counts()));
            }
            { // phm_mt
                auto phm_mt = hist::PartialHistogramManagerMT<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm_mt->get_total_counts()));

                phm_mt = hist::PartialHistogramManagerMT<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm_mt->get_total_counts()));
            }
        }

        SECTION("waters only") {
            // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
            std::vector<Atom> a = {};
            std::vector<Water> w = {Water(Vector3<double>(-1, -1, -1), 1, constants::atom_t::C, "C", 1), Water(Vector3<double>(-1, 1, -1), 1, constants::atom_t::C, "C", 1), 
                                    Water(Vector3<double>( 1, -1, -1), 1, constants::atom_t::C, "C", 1), Water(Vector3<double>( 1, 1, -1), 1, constants::atom_t::C, "C", 1), 
                                    Water(Vector3<double>(-1, -1,  1), 1, constants::atom_t::C, "C", 1), Water(Vector3<double>(-1, 1,  1), 1, constants::atom_t::C, "C", 1),
                                    Water(Vector3<double>( 1, -1,  1), 1, constants::atom_t::C, "C", 1), Water(Vector3<double>( 1, 1,  1), 1, constants::atom_t::C, "C", 1)};
            Molecule protein(a, w);
            set_unity_charge(protein);

            { // hm
                auto hm = hist::HistogramManager<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm->get_total_counts()));

                hm = hist::HistogramManager<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm->get_total_counts()));
            }
            { // hm_mt
                auto hm_mt = hist::HistogramManagerMT<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt->get_total_counts()));

                hm_mt = hist::HistogramManagerMT<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt->get_total_counts()));
            }
            { // hm_mt_ff_avg
                auto hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_avg->get_total_counts()));

                hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_avg->get_total_counts()));
            }
            { // hm_mt_ff_explicit
                auto hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_explicit->get_total_counts()));

                hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_explicit->get_total_counts()));
            }
            // grid-based doesn't make sense for a water-only system and will throw an exception - excluded volume is based on atomic volumes
            // { // hm_mt_ff_grid 
            //     auto hm_mt_ff_grid = hist::HistogramManagerMTFFGrid(&protein).calculate_all();
            //     REQUIRE(compare_hist(p_exp, hm_mt_ff_grid->get_total_counts()));
            // }
            { // phm
                auto phm = hist::PartialHistogramManager<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm->get_total_counts()));

                phm = hist::PartialHistogramManager<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm->get_total_counts()));
            }
            { // phm_mt
                auto phm_mt = hist::PartialHistogramManagerMT<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm_mt->get_total_counts()));

                phm_mt = hist::PartialHistogramManagerMT<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm_mt->get_total_counts()));
            }
        }

        SECTION("both waters and atoms") {
            // the following just describes the eight corners of a cube centered at origo, with an additional atom at the very middle
            std::vector<Atom> b1 = {Atom( Vector3<double>(-1, -1, -1), 1, constants::atom_t::C, "C", 1), Atom( Vector3<double>(-1, 1, -1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Atom> b2 = {Atom( Vector3<double>( 1, -1, -1), 1, constants::atom_t::C, "C", 1), Atom( Vector3<double>( 1, 1, -1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Atom> b3 = {Atom( Vector3<double>(-1, -1,  1), 1, constants::atom_t::C, "C", 1), Atom( Vector3<double>(-1, 1,  1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Water> w = {Water(Vector3<double>( 1, -1,  1), 1, constants::atom_t::C, "C", 1), Water(Vector3<double>( 1, 1,  1), 1, constants::atom_t::C, "C", 1)};
            std::vector<Body> a = {Body(b1), Body(b2), Body(b3)};
            Molecule protein(a, w);
            set_unity_charge(protein);

            { // hm
                auto hm = hist::HistogramManager<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm->get_total_counts()));

                hm = hist::HistogramManager<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm->get_total_counts()));
            }
            { // hm_mt
                auto hm_mt = hist::HistogramManagerMT<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt->get_total_counts()));

                hm_mt = hist::HistogramManagerMT<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt->get_total_counts()));
            }
            { // hm_mt_ff_avg
                auto hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_avg->get_total_counts()));

                hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_avg->get_total_counts()));
            }
            { // hm_mt_ff_explicit
                auto hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_explicit->get_total_counts()));

                hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_explicit->get_total_counts()));
            }
            { // hm_mt_ff_grid
                auto hm_mt_ff_grid = hist::HistogramManagerMTFFGrid<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_grid->get_total_counts()));

                hm_mt_ff_grid = hist::HistogramManagerMTFFGrid<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, hm_mt_ff_grid->get_total_counts()));
            }
            { // phm
                auto phm = hist::PartialHistogramManager<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm->get_total_counts()));

                phm = hist::PartialHistogramManager<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm->get_total_counts()));
            }
            { // phm_mt
                auto phm_mt = hist::PartialHistogramManagerMT<false>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm_mt->get_total_counts()));

                phm_mt = hist::PartialHistogramManagerMT<true>(&protein).calculate_all();
                REQUIRE(compare_hist(p_exp, phm_mt->get_total_counts()));
            }
        }
    }

    SECTION("real data with hydration") {
//        settings::molecule::implicit_hydrogens = false;

        // create the atom, and perform a sanity check on our extracted list
        Molecule protein("test/files/2epe.pdb");
        protein.generate_new_hydration();
        auto p_exp = protein.get_histogram();

        { // hm
            auto hm = hist::HistogramManager<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm->get_total_counts()));

            hm = hist::HistogramManager<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm->get_total_counts()));
        }
        { // hm_mt
            auto hm_mt = hist::HistogramManagerMT<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt->get_total_counts()));

            hm_mt = hist::HistogramManagerMT<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt->get_total_counts()));
        }
        { // hm_mt_ff_avg
            auto hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt_ff_avg->get_total_counts()));

            hm_mt_ff_avg = hist::HistogramManagerMTFFAvg<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt_ff_avg->get_total_counts()));
        }
        { // hm_mt_ff_explicit
            auto hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt_ff_explicit->get_total_counts()));

            hm_mt_ff_explicit = hist::HistogramManagerMTFFExplicit<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt_ff_explicit->get_total_counts()));
        }
        { // hm_mt_ff_grid
            auto hm_mt_ff_grid = hist::HistogramManagerMTFFGrid<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt_ff_grid->get_total_counts()));

            hm_mt_ff_grid = hist::HistogramManagerMTFFGrid<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), hm_mt_ff_grid->get_total_counts()));
        }
        { // phm
            auto phm = hist::PartialHistogramManager<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), phm->get_total_counts()));

            phm = hist::PartialHistogramManager<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), phm->get_total_counts()));
        }
        { // phm_mt
            auto phm_mt = hist::PartialHistogramManagerMT<false>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), phm_mt->get_total_counts()));

            phm_mt = hist::PartialHistogramManagerMT<true>(&protein).calculate_all();
            REQUIRE(compare_hist(p_exp->get_total_counts(), phm_mt->get_total_counts()));
        }
    }
}

TEST_CASE("PartialHistogramManager::get_probe") {
    settings::general::verbose = false;
    Molecule protein("test/files/2epe.pdb");
    auto phm = hist::PartialHistogramManager<false>(&protein);
    auto sm = phm.get_state_manager();

    // check the signalling object is correct
    CHECK(phm.get_probe(0) == sm->get_probe(0)); 

    // check that it links to the state manager
    sm->reset_to_false();
    phm.get_probe(0)->external_change();
    CHECK(sm->is_externally_modified(0));
}

TEST_CASE("PartialHistogramManager::signal_modified_hydration_layer") {
    settings::general::verbose = false;
    Molecule protein("test/files/2epe.pdb");
    auto phm = hist::PartialHistogramManager<false>(&protein);
    auto sm = phm.get_state_manager();
    sm->reset_to_false();
    phm.signal_modified_hydration_layer();
    CHECK(sm->get_modified_hydration());
}