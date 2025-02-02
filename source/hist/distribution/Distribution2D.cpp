/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <hist/distribution/Distribution2D.h>

#include <cmath>

using namespace hist;

Distribution2D::Distribution2D(const WeightedDistribution2D& other) : container::Container2D<constants::axes::d_type>(other.size_x(), other.size_y()) {
    for (std::size_t x = 0; x < size_x(); x++) {
        for (std::size_t y = 0; y < size_y(); y++) {
            index(x, y) = other.index(x, y).value;
        }
    }
}

void Distribution2D::add(unsigned int x, float distance, constants::axes::d_type value) {index(x, std::round(distance)) += value;}
void Distribution2D::add2(unsigned int x, float distance, constants::axes::d_type value) {index(x, std::round(distance)) += 2*value;}
void Distribution2D::add(unsigned int x, int32_t i, constants::axes::d_type value) {index(x, i) += value;}

constants::axes::d_type& Distribution2D::get_content(int i, int j) {return index(i, j);}
const constants::axes::d_type& Distribution2D::get_content(int i, int j) const {return index(i, j);}