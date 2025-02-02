#pragma once

namespace hist {
    class ICompositeDistanceHistogram;
    class ICompositeDistanceHistogramExv;
    class IHistogramManager;
    class DistanceHistogram;
    class Histogram;

    /**
     * @brief A ScatteringProfile is just a (q, I(q)) histogram. 
     */    
    using ScatteringProfile = Histogram;
}