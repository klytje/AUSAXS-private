#pragma once

#include <math/Matrix.h>
#include <utility/Axis.h>
#include <plots/PlotOptions.h>

namespace hist {
    class Histogram2D : public Matrix<double>, public plots::Plottable {
        public: 
            Histogram2D() {}

            Histogram2D(unsigned int size_x, unsigned int size_y) : Matrix(size_x, size_y) {}

            Histogram2D(Axis x_axis, Axis y_axis) : Matrix(x_axis.bins, y_axis.bins), x_axis(x_axis), y_axis(y_axis) {}

            std::string to_string() const;

            Axis x_axis, y_axis;
    };
}