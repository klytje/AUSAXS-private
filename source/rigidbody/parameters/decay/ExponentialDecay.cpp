/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <rigidbody/parameters/decay/ExponentialDecay.h>

#include <cmath>

using namespace rigidbody::parameters::decay;

ExponentialDecay::ExponentialDecay(unsigned int max_iterations) {
    set_characteristic_time(max_iterations/2);
}

ExponentialDecay::~ExponentialDecay() = default;

double ExponentialDecay::get_factor() {
    return std::exp(-decay_rate*draws++);
}

void ExponentialDecay::set_characteristic_time(unsigned int iterations) {
    decay_rate = 1.0/iterations;
}