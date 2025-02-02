/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <rigidbody/selection/RandomSelect.h>
#include <rigidbody/constraints/ConstraintManager.h>
#include <rigidbody/RigidBody.h>
#include <utility/Exceptions.h>

using namespace rigidbody::selection;

RandomSelect::RandomSelect(const RigidBody* rigidbody) : BodySelectStrategy(rigidbody) {
    std::random_device random;
    generator = std::mt19937(random());
    distribution = std::uniform_int_distribution<int>(0, N-1);
}

RandomSelect::~RandomSelect() = default;

std::pair<unsigned int, unsigned int> RandomSelect::next() {
    unsigned int ibody = distribution(generator);

    unsigned int N = rigidbody->get_constraint_manager()->distance_constraints_map.at(ibody).size();
    switch (N) {
        case 0: {
            throw except::invalid_argument("RandomSelect::next: No constraints for body " + std::to_string(ibody));
        }
        case 1: {
            return std::make_pair(ibody, 0);
        }
        default: {
            std::random_device random;
            std::mt19937 generator2(random());
            std::uniform_int_distribution<int> distribution2(0, rigidbody->get_constraint_manager()->distance_constraints_map.at(ibody).size()-1);
            unsigned int iconstraint = distribution2(generator2);

            return std::make_pair(ibody, iconstraint);
        }
    }
}