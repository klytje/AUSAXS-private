#pragma once

#include <rigidbody/parameters/decay/DecayStrategy.h>

namespace rigidbody::parameters::decay {
    class ExponentialDecay : public DecayStrategy {
        public:
            ExponentialDecay(unsigned int max_iterations);
            ~ExponentialDecay();

            double get_factor() override;

            /**
             * @brief Set the number of iterations required to reach an amplitude of 1/e.
             */
            void set_characteristic_time(unsigned int iterations) override;

        private:
            double decay_rate;
    };
}