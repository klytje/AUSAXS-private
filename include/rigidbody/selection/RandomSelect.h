#pragma once

#include <rigidbody/selection/BodySelectStrategy.h>

#include <random>

namespace rigidbody {
    namespace selection {
        /**
         * @brief Thread-safe body selection strategy. The next body is randomly selected, and the next constraint is randomly selected from the constraints connecting to that body.
         */
        class RandomSelect : public BodySelectStrategy {
            public: 
                /**
                 * @brief Constructor.
                 */
                RandomSelect(const RigidBody* rigidbody);

                /**
                 * @brief Destructor.
                 */
                ~RandomSelect() override;

                /**
                 * @brief Get the index of the next body to be transformed. 
                 */
                std::pair<unsigned int, unsigned int> next() override;

            private:
                std::mt19937 generator;                          // The random number generator. 
                std::uniform_int_distribution<int> distribution; // The random number distribution. 
        };
    }
}