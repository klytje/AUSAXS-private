#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <rigidbody/RigidBody.h>
#include <rigidbody/transform/RigidTransform.h>
#include <rigidbody/selection/RandomSelect.h>
#include <rigidbody/selection/RandomConstraintSelect.h>
#include <rigidbody/selection/SequentialSelect.h>
#include <rigidbody/constraints/ConstraintManager.h>
#include <rigidbody/constraints/DistanceConstraint.h>
#include <rigidbody/transform/TransformGroup.h>
#include <fitter/HydrationFitter.h>
#include <data/BodySplitter.h>
#include <data/Protein.h>
#include <data/Body.h>
#include <data/Water.h>
#include <hydrate/Grid.h>
#include <hydrate/GridMember.h>
#include <hydrate/GridObj.h>
#include <settings/All.h>

#include <unordered_map>
#include <unordered_set>

using namespace rigidbody;

TEST_CASE("can_reuse_fitter", "[files]") {
    settings::general::verbose = false;
    Protein protein_2epe("test/files/2epe.pdb");
    Protein protein_LAR12("data/LAR1-2/LAR1-2.pdb");
    protein_2epe.generate_new_hydration();
    protein_LAR12.generate_new_hydration();

    SECTION("intensity_fitter") {
        fitter::HydrationFitter fitter("test/files/2epe.dat", protein_2epe.get_histogram());
        double chi2 = fitter.fit()->fval;

        fitter.set_scattering_hist(protein_LAR12.get_histogram());
        double _chi2 = fitter.fit()->fval;
        REQUIRE_THAT(chi2, !Catch::Matchers::WithinRel(_chi2));

        fitter.set_scattering_hist(protein_2epe.get_histogram());
        _chi2 = fitter.fit()->fval;
        REQUIRE_THAT(chi2, Catch::Matchers::WithinRel(_chi2));
    }

    SECTION("simple_intensity_fitter") {
        fitter::LinearFitter fitter("test/files/2epe.dat", protein_2epe.get_histogram());
        double chi2 = fitter.fit()->fval;

        fitter.set_scattering_hist(protein_LAR12.get_histogram());
        double _chi2 = fitter.fit()->fval;
        REQUIRE_THAT(chi2, !Catch::Matchers::WithinRel(_chi2));

        fitter.set_scattering_hist(protein_2epe.get_histogram());
        _chi2 = fitter.fit()->fval;
        REQUIRE_THAT(chi2, Catch::Matchers::WithinRel(_chi2));
    }
}

TEST_CASE("can_repeat_fit") {
    settings::general::verbose = false;
    Protein protein("test/files/2epe.pdb");
    fitter::LinearFitter fitter("test/files/2epe.dat", protein.get_histogram());

    protein.generate_new_hydration();
    double chi2 = fitter.fit()->fval;

    for (int i = 0; i < 10; i++) {
        protein.generate_new_hydration();
        double _chi2 = fitter.fit()->fval;
        REQUIRE_THAT(chi2, Catch::Matchers::WithinRel(_chi2));
    }
}

TEST_CASE("rigidbody_opt", "[manual]") {
    settings::general::verbose = false;
    std::vector<int> splits = {9, 99};
    Protein protein(BodySplitter::split("data/LAR1-2/LAR1-2.pdb", splits));
    protein.generate_new_hydration();

    fitter::HydrationFitter fitter("data/LAR1-2/LAR1-2.dat", protein.get_histogram());
    fitter.fit()->fval;
}

TEST_CASE("body_selectors") {
    std::vector<Atom> b1 = {Atom(Vector3<double>(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3<double>(-1, 1, -1), 1, "C", "C", 1)};
    std::vector<Atom> b2 = {Atom(Vector3<double>( 1, -1, -1), 1, "C", "C", 1), Atom(Vector3<double>( 1, 1, -1), 1, "C", "C", 1)};
    std::vector<Atom> b3 = {Atom(Vector3<double>(-1, -1,  1), 1, "C", "C", 1), Atom(Vector3<double>(-1, 1,  1), 1, "C", "C", 1)};
    std::vector<Atom> b4 = {Atom(Vector3<double>( 1, -1,  1), 1, "C", "C", 1), Atom(Vector3<double>( 1, 1,  1), 1, "C", "C", 1)};
    std::vector<std::vector<Atom>> atoms = {b1, b2, b3, b4};
    RigidBody rigidbody({atoms, {}});
    auto manager = rigidbody.constraints;

    // add a varying number of constraints to each body
    for (unsigned int i = 0; i < rigidbody.body_size(); i++) {
        for (unsigned int j = i+1; j < rigidbody.body_size(); j++) {
            for (unsigned int k = j; k < 5; k++) {
                manager->add_constraint(rigidbody::DistanceConstraint(&rigidbody, i, j, 0, 0));
            }
        }
    }

    SECTION("RandomSelect") {
        std::unique_ptr<rigidbody::selection::BodySelectStrategy> strat = std::make_unique<rigidbody::selection::RandomSelect>(&rigidbody);
        std::unordered_map<unsigned int, std::unordered_map<unsigned int, unsigned int>> count;

        // count how many times each body and constraint is selected
        unsigned int iterations = 1000;
        for (unsigned int i = 0; i < iterations; i++) {
            auto[ibody, iconstraint] = strat->next();
            if (ibody >= rigidbody.body_size()) {
                std::cout << "Strategy selected a body outside the allowed range. Number: " << ibody << std::endl;
                REQUIRE(false);
            } 
            if (iconstraint >= rigidbody.constraints->distance_constraints_map.at(ibody).size()) {
                std::cout << "Strategy selected a constraint outside the allowed range. Number: " << iconstraint << std::endl;
                REQUIRE(false);
            }
            count[ibody][iconstraint]++;
        }

        for (unsigned int i = 0; i < rigidbody.body_size(); i++) {
            // calculate how many times each body was selected
            double sum = 0;
            for (unsigned int j = 0; j < rigidbody.constraints->distance_constraints_map.at(i).size(); j++) {
                sum += count[i][j];
            }

            // check that each body was selected at least 20% of the time
            REQUIRE(sum > iterations*0.2);

            // check that the constraints were randomly selected
            for (unsigned int j = i; j < rigidbody.constraints->distance_constraints_map.at(i).size(); j++) {
                REQUIRE(count[i][j] > 0.7*sum/rigidbody.constraints->distance_constraints_map.at(i).size());
            }
        }
    }

    SECTION("SequentialSelect") {
        std::unique_ptr<rigidbody::selection::BodySelectStrategy> strat = std::make_unique<rigidbody::selection::SequentialSelect>(&rigidbody);
        std::unordered_map<unsigned int, std::unordered_map<unsigned int, unsigned int>> count;

        // check that the constraints are selected sequentially
        for (unsigned int i = 0; i < rigidbody.body_size(); i++) {
            for (unsigned int j = 0; j < rigidbody.constraints->distance_constraints_map.at(i).size(); j++) {
                auto[ibody, iconstraint] = strat->next();
                REQUIRE(ibody == i);
                REQUIRE(iconstraint == j);
            }
        }

        // check that the strategy loops back to the beginning
        auto[ibody, iconstraint] = strat->next();
        REQUIRE(ibody == 0);
        REQUIRE(iconstraint == 0);
    }

    SECTION("RandomConstraintSelect") {
        std::unique_ptr<rigidbody::selection::BodySelectStrategy> strat = std::make_unique<rigidbody::selection::RandomConstraintSelect>(&rigidbody);
        std::unordered_map<unsigned int, unsigned int> count;

        // count how many times each constraint is selected
        unsigned int iterations = 1000;
        for (unsigned int i = 0; i < iterations; i++) {
            auto[ibody, iconstraint] = strat->next();
            if (ibody >= rigidbody.body_size()) {
                std::cout << "Strategy selected a body outside the allowed range. Number: " << ibody << std::endl;
                REQUIRE(false);
            } 
            if (iconstraint >= rigidbody.constraints->distance_constraints_map.at(ibody).size()) {
                std::cout << "Strategy selected a constraint outside the allowed range. Number: " << iconstraint << std::endl;
                REQUIRE(false);
            }
            count[iconstraint]++;
        }

        for (unsigned int i = 0; i < rigidbody.constraints->distance_constraints.size(); i++) {
            REQUIRE(count[i] > 0.8*iterations/rigidbody.body_size());
        }
    }
}

TEST_CASE("transforms") {
    std::vector<Atom> b0 = {Atom(Vector3<double>(-1, -1, -1), 1, "C", "C", 1), Atom(Vector3<double>(-1, 1, -1), 1, "C", "C", 1)};
    std::vector<Atom> b1 = {Atom(Vector3<double>( 1, -1, -1), 1, "C", "C", 1), Atom(Vector3<double>( 1, 1, -1), 1, "C", "C", 1)};
    std::vector<Atom> b2 = {Atom(Vector3<double>(-1, -1,  1), 1, "C", "C", 1), Atom(Vector3<double>(-1, 1,  1), 1, "C", "C", 1)};
    std::vector<Atom> b3 = {Atom(Vector3<double>( 1, -1,  1), 1, "C", "C", 1), Atom(Vector3<double>( 1, 1,  1), 1, "C", "C", 1)};
    std::vector<Atom> b4 = {Atom(Vector3<double>( 0,  0,  0), 1, "C", "C", 1), Atom(Vector3<double>( 0, 0,  2), 1, "C", "C", 1)};
    std::vector<std::vector<Atom>> atoms = {b0, b1, b2, b3, b4};
    RigidBody rigidbody({atoms, {}});
    auto manager = rigidbody.constraints;

    manager->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 0, 1, 0, 0)); // 0
    manager->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 1, 2, 0, 0)); // 1
    manager->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 2, 3, 0, 0)); // 2
    manager->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 3, 4, 0, 0)); // 3

    SECTION("Single") {
    }

    SECTION("Rigid") {
        auto vector_contains = [] (std::vector<unsigned int> vec, std::vector<unsigned int> vals) {
            std::unordered_set<unsigned int> set(vec.begin(), vec.end());
            for (auto val : vals) {
                if (!set.contains(val)) {
                    return false;
                }
            }
            return true;
        };

        SECTION("get_connected") {
            struct TestRigidTransform : public RigidTransform {
                using RigidTransform::RigidTransform;
                using RigidTransform::get_connected;
            };

            SECTION("simple") {
                TestRigidTransform transform(&rigidbody);

                // 0 - 1 - 2 - 3 - 4            //
                auto group1 = transform.get_connected(rigidbody.constraints->distance_constraints[0]);
                REQUIRE(group1.indices.size() == 1);
                CHECK(group1.indices[0] == 0);

                auto group2 = transform.get_connected(rigidbody.constraints->distance_constraints[1]);
                REQUIRE(group2.indices.size() == 2);
                CHECK(vector_contains(group2.indices, {0, 1}));

                auto group3 = transform.get_connected(rigidbody.constraints->distance_constraints[2]);
                REQUIRE(group3.indices.size() == 2);
                CHECK(vector_contains(group3.indices, {3, 4}));

                auto group4 = transform.get_connected(rigidbody.constraints->distance_constraints[3]);
                REQUIRE(group4.indices.size() == 1);
                CHECK(group4.indices[0] == 4);
            }

            SECTION("complex") {
                std::vector<Atom> b5 = {Atom(Vector3<double>(0, 1, 0), 1, "C", "C", 1), Atom(Vector3<double>(0, 2, 0), 1, "C", "C", 1)};
                std::vector<Atom> b6 = {Atom(Vector3<double>(0, 3, 0), 1, "C", "C", 1), Atom(Vector3<double>(0, 4, 0), 1, "C", "C", 1)};
                std::vector<Atom> b7 = {Atom(Vector3<double>(1, 0, 0), 1, "C", "C", 1), Atom(Vector3<double>(2, 0, 0), 1, "C", "C", 1)};
                atoms = {b0, b1, b2, b3, b4, b5, b6, b7};
                RigidBody rigidbody2({atoms, {}});

                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 0, 1, 0, 0)); // 0
                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 1, 2, 0, 0)); // 1
                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 2, 3, 0, 0)); // 2
                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 3, 4, 0, 0)); // 3
                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 3, 5, 0, 0)); // 4
                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 5, 6, 0, 0)); // 5
                rigidbody2.constraints->add_constraint(rigidbody::DistanceConstraint(&rigidbody, 3, 7, 0, 0)); // 6

                //             5 - 6            //
                //             |                //
                // 0 - 1 - 2 - 3 - 4            //
                //             |                //
                //             7                //

                TestRigidTransform transform(&rigidbody2);
                auto group1 = transform.get_connected(rigidbody2.constraints->distance_constraints[0]);
                REQUIRE(group1.indices.size() == 1);
                CHECK(group1.indices[0] == 0);

                auto group2 = transform.get_connected(rigidbody2.constraints->distance_constraints[1]);
                REQUIRE(group2.indices.size() == 2);
                CHECK(vector_contains(group2.indices, {0, 1}));

                auto group3 = transform.get_connected(rigidbody2.constraints->distance_constraints[2]);
                REQUIRE(group3.indices.size() == 3);
                CHECK(vector_contains(group3.indices, {0, 1, 2}));

                auto group4 = transform.get_connected(rigidbody2.constraints->distance_constraints[3]);
                REQUIRE(group4.indices.size() == 1);
                CHECK(group4.indices[0] == 4);

                auto group5 = transform.get_connected(rigidbody2.constraints->distance_constraints[4]);
                REQUIRE(group5.indices.size() == 2);
                CHECK(vector_contains(group5.indices, {5, 6}));

                auto group6 = transform.get_connected(rigidbody2.constraints->distance_constraints[5]);
                REQUIRE(group6.indices.size() == 1);
                CHECK(group6.indices[0] == 6);

                auto group7 = transform.get_connected(rigidbody2.constraints->distance_constraints[6]);
                REQUIRE(group7.indices.size() == 1);
                CHECK(group7.indices[0] == 7);
            }
        }
    }
}

TEST_CASE("iteration_step") {
    settings::general::verbose = false;
    auto validate_single_step = [] (Protein& protein) {
        protein.generate_new_hydration();

        // fit the protein
        fitter::HydrationFitter fitter("test/files/2epe.dat", protein.get_histogram());
        auto chi2 = fitter.fit()->fval;

        Body&                       body = protein.get_body(0);
        std::shared_ptr<grid::Grid> grid = protein.get_grid();
        Body                        old_body(body);
        grid::Grid                  old_grid = *protein.get_grid();
        Protein                     old_protein(protein);

        //####################################//
        //### do one step of rigidbody opt ###//
        //####################################//
        grid->remove(&body);
        body.translate(Vector3<double>(0, 0, 10));              // translate the body
        body.rotate(Vector3<double>(0, 0, 1), 0.1);             // rotate the body
        grid->add(&body);
        protein.generate_new_hydration();                       // generate a new hydration shell
        fitter.set_scattering_hist(protein.get_histogram());    // update the scattering histogram to reflect the new body positions
        auto _chi2 = fitter.fit()->fval;                        // fit the protein
        CHECK_THAT(chi2, !Catch::Matchers::WithinRel(_chi2));   // chi2 should be different

        //######################################//
        //### reset the state of the protein ###//
        //######################################//
        protein.set_grid(old_grid);                             // reset the grid
        body = std::move(old_body);                             // reset the body
        grid = protein.get_grid();                              // reset the grid pointer

        // check that the grid is exactly identical
        auto axes = grid->get_axes();
        auto oldaxes = old_grid.get_axes();
        REQUIRE(axes == oldaxes);
        for (unsigned int i = 0; i < axes.x.bins; i++) {
            for (unsigned int j = 0; j < axes.y.bins; j++) {
                for (unsigned int k = 0; k < axes.z.bins; k++) {
                    if (grid->grid.index(i, j, k) != old_grid.grid.index(i, j, k)) {
                        std::cout << "Failed on (i, j, k) = (" << i << ", j " << j << ", k " << k << ")" << std::endl;
                        REQUIRE(grid->grid.index(i, j, k) == old_grid.grid.index(i, j, k));
                    }
                }
            }
        }
        REQUIRE(true); // this is just to increment the successful tests counter
        REQUIRE(*grid == old_grid);

        // check that the atoms are the same
        auto atoms = protein.get_atoms();
        auto oldatoms = old_protein.get_atoms();
        REQUIRE(atoms.size() == oldatoms.size());
        for (unsigned int i = 0; i < atoms.size(); i++) {
            if (atoms.at(i).coords != oldatoms.at(i).coords) {
                std::cout << "Failed on atom " << i << std::endl;
                std::cout << "Old coords: " << oldatoms.at(i).coords << std::endl;
                std::cout << "New coords: " << atoms.at(i).coords << std::endl;
                REQUIRE(atoms.at(i).coords == oldatoms.at(i).coords);
            }
        }
        REQUIRE(true);

        // check that the grid water members are the same
        auto wm = grid->w_members;
        auto oldwm = old_grid.w_members;
        REQUIRE(wm.size() == oldwm.size());

        auto wm_it = wm.begin();
        auto oldwm_it = oldwm.begin();
        while (wm_it != wm.end()) {
            if (*wm_it != *oldwm_it) {
                REQUIRE(*wm_it == *oldwm_it);
            }
            wm_it++;
            oldwm_it++;
        }
        REQUIRE(true);

        // check that the grid atom members are the same
        auto am = grid->a_members;
        auto oldam = old_grid.a_members;
        REQUIRE(am.size() == oldam.size());

        auto am_it = am.begin();
        auto oldam_it = oldam.begin();
        while (am_it != am.end()) {
            if (*am_it != *oldam_it) {
                REQUIRE(*am_it == *oldam_it);
            }
            am_it++;
            oldam_it++;
        }
        REQUIRE(true);

        // check that there's no water in the grid after a call to clear_hydration
        protein.clear_hydration();
        for (unsigned int i = 0; i < axes.x.bins; i++) {
            for (unsigned int j = 0; j < axes.y.bins; j++) {
                for (unsigned int k = 0; k < axes.z.bins; k++) {
                    if (grid->grid.index(i, j, k) == grid::GridObj::H_CENTER) {
                        std::cout << "Failed on (i, j, k) = (" << i << ", j " << j << ", k " << k << ")" << std::endl;
                        REQUIRE(grid->grid.index(i, j, k) != grid::GridObj::H_CENTER);
                    }
                    if (grid->grid.index(i, j, k) == grid::GridObj::H_AREA) {
                        std::cout << "Failed on (i, j, k) = (" << i << ", j " << j << ", k " << k << ")" << std::endl;
                        REQUIRE(grid->grid.index(i, j, k) != grid::GridObj::H_AREA);
                    }
                }
            }
        }
        REQUIRE(true);

        //################################################//
        //### check that the protein has been reverted ###//
        //################################################//
        protein.generate_new_hydration();                       // generate a new hydration shell
        fitter.set_scattering_hist(protein.get_histogram());    // update the scattering histogram to reflect the new body positions
        _chi2 = fitter.fit()->fval;                             // fit the protein
        CHECK_THAT(chi2, Catch::Matchers::WithinRel(_chi2));    // chi2 should be the same

        // check that the waters are the same
        auto newwaters = protein.get_waters();
        auto oldwaters = old_protein.get_waters();
        REQUIRE(newwaters.size() == oldwaters.size());
        for (unsigned int i = 0; i < newwaters.size(); i++) {
            if (newwaters.at(i).coords != oldwaters.at(i).coords) {
                std::cout << "Failed on water " << i << std::endl;
                std::cout << "Old coords: " << oldwaters.at(i).coords << std::endl;
                std::cout << "New coords: " << newwaters.at(i).coords << std::endl;
                REQUIRE(oldwaters.at(i).coords == newwaters.at(i).coords);
            }
        }
        REQUIRE(true);

        // check that the grid is the same
        for (unsigned int i = 0; i < axes.x.bins; i++) {
            for (unsigned int j = 0; j < axes.y.bins; j++) {
                for (unsigned int k = 0; k < axes.z.bins; k++) {
                    if (grid->grid.index(i, j, k) != old_grid.grid.index(i, j, k)) {
                        std::cout << "Failed on (i, j, k) = (" << i << ", j " << j << ", k " << k << ")" << std::endl;
                        REQUIRE(grid->grid.index(i, j, k) == old_grid.grid.index(i, j, k));
                    }
                }
            }
        }
        REQUIRE(true);
        REQUIRE(*grid == old_grid);
    };

    SECTION("simple") {
        Atom a1(1, "C", "", "LYS", "", 1, "", Vector3<double>(-1, -1, -1), 1, 0, "C", "");
        Atom a2(2, "C", "", "LYS", "", 1, "", Vector3<double>(-1, -1,  1), 1, 0, "C", "");
        Atom a3(3, "C", "", "LYS", "", 1, "", Vector3<double>(-1,  1, -1), 1, 0, "C", "");
        Atom a4(4, "C", "", "LYS", "", 1, "", Vector3<double>(-1,  1,  1), 1, 0, "C", "");
        Atom a5(5, "C", "", "LYS", "", 1, "", Vector3<double>( 1, -1, -1), 1, 0, "C", "");
        Atom a6(6, "C", "", "LYS", "", 1, "", Vector3<double>( 1, -1,  1), 1, 0, "C", "");
        Atom a7(7, "C", "", "LYS", "", 1, "", Vector3<double>( 1,  1, -1), 1, 0, "C", "");
        Atom a8(8, "C", "", "LYS", "", 1, "", Vector3<double>( 1,  1,  1), 1, 0, "C", "");

        Body b1(std::vector<Atom>{a1, a2});
        Body b2(std::vector<Atom>{a3, a4});
        Body b3(std::vector<Atom>{a5, a6});
        Body b4(std::vector<Atom>{a7, a8});
        std::vector<Body> ap = {b1, b2, b3, b4};
        Protein protein(ap);
        grid::Grid grid(Axis3D(-20, 20, -20, 20, -20, 20, 1));
        grid.add(protein.get_atoms());
        protein.set_grid(grid);

        validate_single_step(protein);
    }

    SECTION("real data") {
        Protein protein = BodySplitter::split("data/lysozyme/2epe.pdb", {9, 99});
        REQUIRE(protein.body_size() == 3);
        validate_single_step(protein);
    }
}

class RigidBodyTest : public RigidBody {
    public: 
        using RigidBody::RigidBody;
        using RigidBody::transform;
};

TEST_CASE("can_find_optimal_conformation") {
    settings::grid::cubic = true;
    settings::grid::scaling = 2;
    settings::rigidbody::iterations = 1000;
    RigidBodyTest body = BodySplitter::split("test/files/LAR1-2.pdb", {2, 9, 99, 194});

    SECTION("test 1") {
        body.transform->apply(matrix::rotation_matrix({0, 1, 0}, M_PI_2), {0, 0, 0}, body.constraints->distance_constraints[0]);
        body.transform->apply(matrix::rotation_matrix({0, 0, 1}, M_PI),   {0, 0, 0}, body.constraints->distance_constraints[1]);
        body.transform->apply(matrix::rotation_matrix({1, 0, 0}, M_PI_4), {0, 0, 0}, body.constraints->distance_constraints[2]);
        auto hist = body.get_histogram().calc_debye_scattering_intensity();
        hist.reduce(100);
        hist.simulate_errors();
        hist.simulate_noise();
        hist.save("temp/rigidbody/test1.dat");
        body.save("temp/rigidbody/test1.pdb");

        RigidBody body2 = BodySplitter::split("test/files/LAR1-2.pdb", {2, 9, 99, 194});
        auto res = body2.optimize("temp/rigidbody/test1.dat");
        REQUIRE(res->fval/res->dof < 2);
    }

    SECTION("test 2") {
        body.transform->apply(matrix::rotation_matrix({1, 1, 1}, M_PI_2), {0, 0, 0}, body.constraints->distance_constraints[0]);
        body.transform->apply(matrix::rotation_matrix({1, 0, 1}, M_PI_2), {0, 0, 0}, body.constraints->distance_constraints[1]);
        body.transform->apply(matrix::rotation_matrix({0, 1, 1}, M_PI_2), {0, 0, 0}, body.constraints->distance_constraints[2]);
        auto hist = body.get_histogram().calc_debye_scattering_intensity();
        hist.reduce(100);
        hist.simulate_errors();
        hist.simulate_noise();
        hist.save("temp/rigidbody/test2.dat");
        body.save("temp/rigidbody/test2.pdb");

        RigidBody body2 = BodySplitter::split("test/files/LAR1-2.pdb", {2, 9, 99, 194});
        auto res = body2.optimize("temp/rigidbody/test2.dat");
        REQUIRE(res->fval/res->dof < 2);
    }

    SECTION("test 3") {
        body.transform->apply(matrix::rotation_matrix({1, 1, 1}, M_PI_2), {10, 0, 0}, body.constraints->distance_constraints[0]);
        body.transform->apply(matrix::rotation_matrix({1, 0, 1}, 0), {0, 10, 0},      body.constraints->distance_constraints[1]);
        body.transform->apply(matrix::rotation_matrix({0, 1, 1}, 0), {0, 0, 10},      body.constraints->distance_constraints[2]);
        auto hist = body.get_histogram().calc_debye_scattering_intensity();
        hist.reduce(100);
        hist.simulate_errors();
        hist.simulate_noise();
        hist.save("temp/rigidbody/test3.dat");
        body.save("temp/rigidbody/test3.pdb");

        RigidBody body2 = BodySplitter::split("test/files/LAR1-2.pdb", {2, 9, 99, 194});
        auto res = body2.optimize("temp/rigidbody/test3.dat");
        REQUIRE(res->fval/res->dof < 2);
    }
}

// TEST_CASE("generate_sequential_constraints", "[body],[files]") {
//     settings::general::verbose = false;
//     vector<int> splits = {9, 99};
//     Protein protein = rigidbody::BodySplitter::split("data/LAR1-2/LAR1-2.pdb", splits);
//     vector<rigidbody::Constraint> constraints = rigidbody::BodySplitter::sequential_constraints(protein);

//     REQUIRE(constraints.size() == 2);

//     // check first constraint
//     rigidbody::Constraint& c1 = constraints[0];
//     REQUIRE(c1.get_atom1().name == "CA");
//     REQUIRE(c1.get_atom1().serial == 131);
//     REQUIRE(c1.get_atom2().name == "CA");
//     REQUIRE(c1.get_atom2().serial == 138);

//     // check second constraint
//     rigidbody::Constraint& c2 = constraints[1];
//     REQUIRE(c2.get_atom1().name == "CA");
//     REQUIRE(c2.get_atom1().serial == 809);
//     REQUIRE(c2.get_atom2().name == "CA");
//     REQUIRE(c2.get_atom2().serial == 814);
// }