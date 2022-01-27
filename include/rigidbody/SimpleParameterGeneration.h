#pragma once

#include <random>

#include "rigidbody/ParameterGenerationStrategy.h"

class SimpleParameterGeneration : public ParameterGenerationStrategy {
  public: 
    /**
     * @brief Constructor.
     * 
     * @param iterations The expected number of iterations. 
     * @param length_start The start length of the generated translation vectors. 
     * @param rad_start The start angle in radians of the generated rotations. 
     */
    SimpleParameterGeneration(const int iterations, const double length_start, const double rad_start) : ParameterGenerationStrategy(iterations, length_start, rad_start) {}

    /**
     * @brief Destructor.
     */
    ~SimpleParameterGeneration() override = default;

    double scale() const {
        return double(iterations - iteration)/iterations; 
    }

    std::tuple<double, double, double> get_rotation() override {
        double scaling = scale();

        double dr1 = rotation_dist(generator)*scaling;
        double dr2 = rotation_dist(generator)*scaling;
        double dr3 = rotation_dist(generator)*scaling;
        return std::tuple(dr1, dr2, dr3);
    }

    Vector3 get_translation() override {       
        double scaling = scale();

        double dx = translation_dist(generator)*scaling;
        double dy = translation_dist(generator)*scaling;
        double dz = translation_dist(generator)*scaling;
        return Vector3(dx, dy, dz);
    }
};