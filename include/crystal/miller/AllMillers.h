#pragma once

#include <crystal/miller/MillerGenerationStrategy.h>

namespace crystal {
    /**
     * @brief Generates all miller indices within the range specified by
     *          settings::crystal::h
     *          settings::crystal::k
     *          settings::crystal::l
     * 
     *        The maximum allowed length of the indices is specified by
     *          settings::crystal::max_q
     *        which guarantees that the indices spans a spherical volume.
     */
    class AllMillers : public MillerGenerationStrategy {
        public: 
            AllMillers(unsigned int h, unsigned int k, unsigned int l);

            std::vector<Miller> generate() const override;
        private: 
            int h, k, l;
    };
}