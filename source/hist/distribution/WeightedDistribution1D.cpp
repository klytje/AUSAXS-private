#include <hist/distribution/WeightedDistribution1D.h>
#include <hist/distribution/Distribution1D.h>
#include <constants/Constants.h>

#include <cmath>

using namespace hist;

WeightedDistribution1D::WeightedDistribution1D(const Distribution1D& other) : Container1D(other.size()) {
    for (std::size_t i = 0; i < other.size(); i++) {
        index(i).count = other.index(i);
    }
}

WeightedDistribution1D::WeightedDistribution1D(const std::vector<constants::axes::d_type>& bins) : WeightedDistribution1D(Distribution1D(bins)) {}

std::vector<constants::axes::d_type> WeightedDistribution1D::as_vector() const {
    return get_bins();
}

void WeightedDistribution1D::add(float distance, constants::axes::d_type value) {
    int i = std::round(distance*constants::axes::d_inv_width);
    index(i).count += value;
    index(i).content += distance;
}

std::vector<constants::axes::d_type> WeightedDistribution1D::get_bins() const {
    Distribution1D bins(size());
    for (std::size_t i = 0; i < size(); i++) {
        bins.index(i) = index(i).count;
    }
    return bins;
}

std::vector<double> WeightedDistribution1D::get_weights() const {
    Distribution1D weights(size());
    for (std::size_t i = 0; i < size(); i++) {
        weights.index(i) = index(i).content;
    }
    return weights;
}