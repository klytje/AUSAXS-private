#pragma once

#include <rigidbody/constraints/generation/ConstraintGenerationStrategy.h>

namespace rigidbody {
    class LinearConstraints : public ConstraintGenerationStrategy {
        public:
            using ConstraintGenerationStrategy::ConstraintGenerationStrategy;

            /**
             * @brief Generate a constraint.
             */
            std::vector<std::shared_ptr<DistanceConstraint>> generate() const override;
    };
}