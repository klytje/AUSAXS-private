#pragma once

#include <math/Matrix.h>
#include <math/Vector.h>

/**
 * @brief \class CubicSpline
 * 
 * Interpolate points using a cubic spline. 
 * Implementation based on some lecture notes from Aarhus University. 
 */
class CubicSpline {
    public:
        CubicSpline(const std::vector<double>& x, const std::vector<double>& y) : x(x), y(y) {setup();}
        CubicSpline(const Vector<double>& x, const Vector<double>& y) : x(x), y(y) {setup();}

        double spline(const double& z) const {
            int i = search(0, x.size(), z);
            return y[i] + b[i]*(z - x[i]) + c[i]*pow(z - x[i], 2) + d[i]*pow(z - x[i], 3);
        }

    private: 
        const Vector<double> x, y;
        Vector<double> b, c, d;

        void setup() {
            int n = x.size();
            b = Vector<double>(n);
            c = Vector<double>(n-1);
            d = Vector<double>(n-1);
            Vector<double> D(n);
            Vector<double> Q(n-1);
            Vector<double> B(n);
            Vector<double> h(n-1);
            Vector<double> p(n-1);
            for (int i = 0; i < n-1; i++) {
                h[i] = x[i+1]-x[i]; // definition of h (eq 15)
                p[i] = (y[i+1]-y[i])/h[i]; // definition of p (eq 6)
            }
            // setting up our known initial values (eq 21 - 23)
            D[0] = 2; Q[0] = 1; B[0] = 3*p[0]; D[n-1] = 2; B[n-1] = 3*p[n-2];
            // recursive relations described by the same set of equations
            for (int i = 0; i < n-2; i++) {
                D[i+1] = 2*h[i]/h[i+1] + 2;
                Q[i+1] = h[i]/h[i+1];
                B[i+1] = 3*(p[i] + p[i+1]*h[i]/h[i+1]);
            }
            for (int i = 1; i < n; i++) {
                D[i] -= Q[i-1]/D[i-1]; // converting D to Dtilde (eq 25)
                B[i] -= B[i-1]/D[i-1]; // converting B to Btilde (eq 26)
            }
            b[n-1] = B[n-1]/D[n-1]; // definition of b (eq 27)
            for (int i = n-2; 0 <= i; i--) {
                b[i] = (B[i] - Q[i]*b[i+1])/D[i]; // definition of b (eq 27)
            }
            for (int i = 0; i < n-1; i++) {
                c[i] = (-2*b[i] - b[i+1] + 3*p[i])/h[i]; // definition of c (eq 18)
                d[i] = (b[i] + b[i+1] - 2*p[i])/pow(h[i], 2); // definition of d (eq 18)
            }
        }

        /**
         * @brief Find the index @p z is supposed to be at in x.
         * @param l left bound
         * @param r right bound
         * @param z the new point on the x-axis
         */
        int search(int l, int r, double z) const {
            int mid = l+(r-l)/2; // middle index to compare with
            if (l == r)
                return std::max(l-1, 0);
            if (z < x[mid])
                return search(l, mid, z);
            return search(mid+1, r, z);
        }
};