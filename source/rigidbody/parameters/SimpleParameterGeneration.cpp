/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <rigidbody/parameters/SimpleParameterGeneration.h>
#include <math/Vector3.h>

#include <random>

using namespace rigidbody::parameter;

SimpleParameterGeneration::SimpleParameterGeneration(int iterations, double length_start, double rad_start) : ParameterGenerationStrategy(iterations, length_start, rad_start) {}

SimpleParameterGeneration::~SimpleParameterGeneration() = default;

std::tuple<double, double, double> SimpleParameterGeneration::get_rotation() {
    double scaling = scale();

    double dr1 = rotation_dist(generator)*scaling;
    double dr2 = rotation_dist(generator)*scaling;
    double dr3 = rotation_dist(generator)*scaling;
    return std::tuple(dr1, dr2, dr3);
}

Vector3<double> SimpleParameterGeneration::get_translation() {
    double scaling = scale();

    double dx = translation_dist(generator)*scaling;
    double dy = translation_dist(generator)*scaling;
    double dz = translation_dist(generator)*scaling;
    return Vector3(dx, dy, dz);
}

double SimpleParameterGeneration::scale() const {
    return double(iterations - iteration)/iterations; 
}