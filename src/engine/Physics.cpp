#include <cmath>
#include "Physics.h"

std::pair<double, double> trippin::elasticCollision1D(double v1, double v2, double m1, double m2) {
    auto v1p = (m1 - m2) / (m1 + m2) * v1 + (2 * m2) / (m1 + m2) * v2;
    auto v2p = (2 * m1) / (m1 + m2) * v1 + (m2 - m1) / (m1 + m2) * v2;
    return {v1p, v2p};
}

std::pair<trippin::DoubleVector, trippin::DoubleVector> trippin::elasticCollision2D(
        trippin::DoubleVector v1,
        trippin::DoubleVector v2,
        trippin::DoubleVector p1,
        trippin::DoubleVector p2,
        double m1,
        double m2) {
    auto pDiff = p1 - p2;
    auto vDiff = v1 - v2;
    auto dot = vDiff * pDiff;
    auto magSq = pDiff.sumOfSquares();
    auto div = dot / magSq;
    auto mdiv = (2 * m2) / (m1 + m2);
    auto prod = pDiff * div * mdiv;
    auto v1p = v1 - prod;

    pDiff = p2 - p1;
    vDiff = v2 - v1;
    dot = vDiff * pDiff;
    magSq = pDiff.sumOfSquares();
    div = dot / magSq;
    mdiv = (2 * m1) / (m1 + m2);
    prod = pDiff * div * mdiv;
    auto v2p = v2 - prod;

    return {v1p, v2p};
}

std::pair<trippin::DoubleVector, trippin::DoubleVector> trippin::inelasticCollision2D(
        trippin::DoubleVector v1,
        trippin::DoubleVector v2,
        trippin::DoubleVector p1,
        trippin::DoubleVector p2,
        double m1,
        double m2,
        double restitutionCoefficient) {
    double xDiff = p2.x - p1.x;
    double yDiff = p2.y - p1.y;
    double r = std::sqrt(xDiff * xDiff + yDiff * yDiff) / 2;
    int error;
    collision2D('x', 0, restitutionCoefficient, m1, m2, r, r, p1.x, p1.y, p2.x, p2.y, v1.x, v1.y, v2.x, v2.y, error);
    return {v1, v2};
}