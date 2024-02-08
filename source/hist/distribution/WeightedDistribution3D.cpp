#include <hist/distribution/WeightedDistribution3D.h>
#include <hist/distribution/Distribution3D.h>
#include <constants/Constants.h>

#include <cmath>

using namespace hist;

WeightedDistribution3D::WeightedDistribution3D(const Distribution3D& other) : Container3D(other.size_x(), other.size_y(), other.size_z()) {
    // std::transform(other.begin(), other.end(), begin(), begin(), [] (const auto& val1, auto& val2) {return val2.count = val2;});
    for (std::size_t x = 0; x < other.size_x(); x++) {
        for (std::size_t y = 0; y < other.size_y(); y++) {
            for (std::size_t z = 0; z < other.size_z(); z++) {
                index(x, y, z).count = other.index(x, y, z);
            }
        }
    }
}

void WeightedDistribution3D::add(int x, int y, float distance, constants::axes::d_type value) {
    int i = std::round(distance*constants::axes::d_inv_width);
    index(x, y, i) += value;
    index(x, y, i).add(distance);
}

std::vector<double> WeightedDistribution3D::get_weights() const {
    std::vector<double> weights(size_z());
    for (std::size_t z = 0; z < size_z(); z++) {
        unsigned int count = 0;
        for (std::size_t x = 0; x < size_x(); x++) {
            for (std::size_t y = 0; y < size_y(); y++) {
                weights[z] = index(x, y, z).content;
                count += index(x, y, z).count;
            }
        }
        weights[z] /= (!count + count); // avoid division by zero
    }
    return weights;
}