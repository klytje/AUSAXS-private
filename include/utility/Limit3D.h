#pragma once

#include <utility/Limit.h>

/**
 * @brief \class Limit3D
 * 
 * A representation of a 3-dimensional limited span of values. 
 */
class Limit3D {
    public:
        Limit3D() noexcept {}
        /**
         * @brief Constructor.
         * 
         * Construct a new 3D limit based on three 1D limits. 
         * 
         * @param x The limit on the x-axis. 
         * @param y The limit on the y-axis. 
         * @param z The limit on the z-axis. 
         */
        Limit3D(const Limit& x, const Limit& y, const Limit& z) noexcept : x(x), y(y), z(z) {}

        /**
         * @brief Constructor. 
         * 
         * Construct a new 3D limit based on the minimum and maximum values along each axis. 
         */
        Limit3D(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax) noexcept : x(xmin, xmax), y(ymin, ymax), z(zmin, zmax) {}

        /**
         * @brief Check if this object is fully initialized. Returns false if any of its Limits are empty.
         */
        [[nodiscard]] 
        bool empty() const noexcept {return x.empty() || y.empty() || z.empty();}

        Limit x; // The 1D limit on the x-axis. 
        Limit y; // The 1D limit on the y-axis. 
        Limit z; // The 1D limit on the z-axis. 
};