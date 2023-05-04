#pragma once

namespace rigidbody::parameters::decay {
    /**
     * @brief The strategy for decaying the amplitude of a parameter.
     */
    class DecayStrategy {
        public:
            virtual ~DecayStrategy() = default;

            /**
             * @brief Get the factor by which to decay the parameter.
             */
            virtual double get_factor() = 0;

            /**
             * @brief Set the characteristic time scale for this decay strategy.
             */
            virtual void set_characteristic_time(unsigned int iterations) = 0;

        protected:
            unsigned int draws;
    };
}