#pragma once

#include <container/Container3D.h>
#include <constants/Axes.h>
#include <hist/distribution/detail/WeightedEntry.h>

namespace hist {
    class Distribution3D;

    /**
     * @brief This is a small wrapper around the Container3D class, indicating that the data
     *        is distributed along the constants::axes::d_vals axis. Anything added to this
     *        distribution will be tracked by the WeightedDistribution class, which may add
     *        a significant overhead compared to a pure Distribution1D class.
     */
    class WeightedDistribution3D : public container::Container3D<detail::WeightedEntry> {
        public:
            using Container3D::Container3D;
            WeightedDistribution3D(const Distribution3D& other);

            /**
             * @brief Add twice the value for a given distance.
             * 
             * @param x The first form factor index.
             * @param y The second form factor index.
             * @param distance The distance to add the value to.
             * @param value The value to add.
             */
            void add(unsigned int x, unsigned int y, float distance, constants::axes::d_type value);

            /**
             * @brief Add twice the value for a given distance.
             * 
             * @param x The first form factor index.
             * @param y The second form factor index.
             * @param distance The distance to add the value to.
             * @param value The value to add.
             */
            void add2(unsigned int x, unsigned int y, float distance, constants::axes::d_type value);

            /**
             * @brief Extract the weights from this distribution.
             */
            std::vector<double> get_weights() const;
    };
}