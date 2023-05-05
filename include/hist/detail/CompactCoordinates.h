#pragma once

template<typename T> class Vector3;
class Body;
class Water;
namespace hist {
    namespace detail {
        /**
         * @brief A compact vector representation of the coordinates and weight of all atoms in a body. 
         *        The idea is that by only extracting the absolute necessities for the distance calculation, more values can be stored
         *        in the cache at any given time. This is meant as a helper class to DistanceCalculator.
         */
        struct CompactCoordinates {
            struct Data {
                Data();
                Data(const Vector3<double>& v, float w);
                float x, y, z, w;
            };

            CompactCoordinates() {}

            /**
             * @brief Extract the necessary coordinates and weights from a body. 
             */
            CompactCoordinates(const Body& body);

            /**
             * @brief Extract the necessary coordinates and weights from a vector of hydration atoms. 
             */
            CompactCoordinates(const std::vector<Water>& atoms);

            unsigned int size;
            std::vector<Data> data;
        };
    }
}