#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <data/StateManager.h>
#include <data/Protein.h>
#include <em/ImageStack.h>
#include <em/PartialHistogramManager.h>

#include <iostream>

using std::vector, std::shared_ptr, std::cout, std::endl;

TEST_CASE("state_manager", "[managers]") {
    unsigned int size = 5;
    StateManager manager(size);

    CHECK(manager.get_externally_modified_bodies() == vector{true, true, true, true, true});
    manager.reset();
    CHECK(manager.get_externally_modified_bodies() == vector{false, false, false, false, false});
    manager.get_probe(2)->external_change();
    manager.get_probe(4)->external_change();
    CHECK(manager.get_externally_modified_bodies() == vector{false, false, true, false, true});
    CHECK(!manager.is_externally_modified(0));
    CHECK(!manager.is_externally_modified(1));
    CHECK(manager.is_externally_modified(2));
    CHECK(!manager.is_externally_modified(3));
    CHECK(manager.is_externally_modified(4));
}

TEST_CASE("partial_histogram_manager_works", "[managers]") {
    vector<Body> bodies(5);
    Protein protein(bodies);
    shared_ptr<hist::PartialHistogramManager> phm = protein.get_histogram_manager();
    StateManager& manager = phm->get_state_manager();

    manager.reset();
    phm->get_probe(0)->external_change();
    phm->get_probe(2)->external_change();
    CHECK(manager.get_externally_modified_bodies() == vector{true, false, true, false, false});
}

TEST_CASE("protein_manager", "[managers]") {
    vector<Body> bodies(5);
    Protein protein(bodies);

    shared_ptr<hist::PartialHistogramManager> phm = protein.get_histogram_manager();
    StateManager& manager = phm->get_state_manager();

    manager.reset();
    CHECK(manager.get_externally_modified_bodies() == vector{false, false, false, false, false});
    shared_ptr<StateManager::BoundSignaller> probe0 = phm->get_probe(0);
    shared_ptr<StateManager::BoundSignaller> probe2 = phm->get_probe(2);
    probe0->external_change();
    probe2->external_change();
    CHECK(manager.get_externally_modified_bodies() == vector{true, false, true, false, false});

    manager.reset();
    CHECK(manager.get_externally_modified_bodies() == vector{false, false, false, false, false});
    Body body;
    body.register_probe(probe0);
    body.changed_external_state();
    CHECK(manager.get_externally_modified_bodies() == vector{true, false, false, false, false});

    manager.reset();
    CHECK(manager.get_externally_modified_bodies() == vector{false, false, false, false, false});
    protein.bind_body_signallers();
    protein.bodies[0].changed_external_state();
    protein.bodies[2].changed_external_state();
    CHECK(manager.get_externally_modified_bodies() == vector{true, false, true, false, false});

    manager.reset();
    CHECK(manager.get_externally_modified_bodies() == vector{false, false, false, false, false});
    protein.bodies[0] = Body();
    protein.bodies[4] = Body();
    protein.bodies[0].changed_external_state();
    protein.bodies[4].changed_external_state();
    CHECK(manager.get_externally_modified_bodies() == vector{true, false, false, false, true});
}

TEST_CASE("partial_histogram_manager", "[managers]") {
    setting::protein::use_effective_charge = false;

    auto compare = [] (em::PartialHistogramManager& manager, double cutoff) {
        hist::ScatteringHistogram h1 = manager.get_histogram_slow(cutoff);
        hist::ScatteringHistogram h2 = manager.get_histogram(cutoff);

        if (h1.p.size() != h2.p.size()) {
            unsigned int lim = std::min(h1.p.size(), h2.p.size());
            for (unsigned int i = 0; i < lim; i++) {
                std::cout << "h1: " << h1.p[i] << ", h2: " << h2.p[i] << std::endl;
            }
            cout << "Unequal sizes. " << endl;
            return false;
        }
        for (unsigned int i = 0; i < h1.p.size(); i++) {
            if (h1.p[i] != h2.p[i]) {
                cout << "Failed on index " << i << ". Values: " << h1.p[i] << ", " << h2.p[i] << endl;
                return false;
            }
        }
        return true;
    };

    SECTION("basic functionality works") {
        std::shared_ptr<em::ccp4::Header> header = std::make_shared<em::ccp4::Header>();
        header->cella_x = 1, header->cella_y = 1, header->cella_z = 1, header->nz = 1;

        Matrix data = Matrix<float>{{1, 2, 3, 4, 5, 6}, {0.5, 1.5, 2.5, 3.5, 4.5, 5.5}};
        em::Image image(data, header, 0);
        em::ImageStack images({image});

        em::PartialHistogramManager manager(images);
        manager.set_charge_levels({2, 4, 6, 8});
        std::shared_ptr<Protein> protein = manager.get_protein(0);

        REQUIRE(protein->body_size() == 5);
        CHECK(protein->bodies[0].atoms().size() == 3);
        CHECK(protein->bodies[1].atoms().size() == 4);
        CHECK(protein->bodies[2].atoms().size() == 4);
        CHECK(protein->bodies[3].atoms().size() == 1);
        CHECK(protein->bodies[4].atoms().size() == 0);

        protein = manager.get_protein(3);
        REQUIRE(protein->body_size() == 5);
        CHECK(protein->bodies[0].atoms().size() == 0);
        CHECK(protein->bodies[1].atoms().size() == 2);
        CHECK(protein->bodies[2].atoms().size() == 4);
        CHECK(protein->bodies[3].atoms().size() == 1);
        CHECK(protein->bodies[4].atoms().size() == 0);
    }

    SECTION("simple comparison with standard approach") {
        SECTION("positive") {
            std::shared_ptr<em::ccp4::Header> header = std::make_shared<em::ccp4::Header>();
            header->cella_x = 1, header->cella_y = 1, header->cella_z = 1, header->nz = 1;

            Matrix data = Matrix<float>{{1, 2, 3, 4, 5, 6}, {0.5, 1.5, 2.5, 3.5, 4.5, 5.5}};
            em::Image image(data, header, 0);
            em::ImageStack images({image});

            em::PartialHistogramManager manager(images);
            manager.set_charge_levels({2, 4, 6, 8});

            // try an arbitrary cutoff level
            REQUIRE(compare(manager, 3));

            // try a lower cutoff level
            REQUIRE(compare(manager, 1));

            // try a higher cutoff level
            REQUIRE(compare(manager, 4));

            // some more tests
            REQUIRE(compare(manager, 5));
            REQUIRE(compare(manager, 2));
            REQUIRE(compare(manager, 3.6));
            REQUIRE(compare(manager, 1));
        }

        SECTION("real example") {
            em::ImageStack images("data/maptest.ccp4");
            auto manager = *images.get_histogram_manager();

            REQUIRE(compare(manager, 4));
            REQUIRE(compare(manager, 3));
            REQUIRE(compare(manager, 2));
            REQUIRE(compare(manager, 5));
            REQUIRE(compare(manager, 6));
        }
    }

    SECTION("comparison with standard approach") {
        setting::em::sample_frequency = 2;
        em::ImageStack images("data/emd_12752/emd_12752.map");
        auto manager = *images.get_histogram_manager();

        REQUIRE(compare(manager, 0.04));
        REQUIRE(compare(manager, 0.03));
        REQUIRE(compare(manager, 0.02));
        REQUIRE(compare(manager, 0.05));
        REQUIRE(compare(manager, 0.06));
    }
}