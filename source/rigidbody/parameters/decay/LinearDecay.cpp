/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <rigidbody/parameters/decay/LinearDecay.h>

using namespace rigidbody::parameters::decay;

LinearDecay::LinearDecay(unsigned int max_iterations) {
    set_characteristic_time(max_iterations/2);
}

LinearDecay::~LinearDecay() = default;

double LinearDecay::get_factor() {
    return 1.0 - decay_rate*draws++;
}

void LinearDecay::set_characteristic_time(unsigned int iterations) {
    decay_rate = 0.5/iterations;
}