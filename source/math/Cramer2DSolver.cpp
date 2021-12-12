#include "math/Matrix.h"
#include "math/Vector.h"
#include "math/LinearSolver.h"

#include <math.h>
#include <stdexcept>

class Cramer2DSolver : public LinearSolver {
public:
    Cramer2DSolver(const Matrix& A) : A(A), N(A.N), M(A.M) {}
    ~Cramer2DSolver() override {}

    Vector solve(const Vector& v) const override {
        double a = A[0][0], b = A[0][1], c = A[1][0], d = A[1][1];
        double det = a*d - b*c;
        if (__builtin_expect(det == 0, false)) {throw std::invalid_argument("Determinant is zero, cannot solve equation.");}

        double x = (v[0]*d - v[1]*b)/det;
        double y = (v[1]*a - v[0]*c)/det;
        return Vector({x, y});
    }

private: 
    const Matrix A;
    const int N, M;
};