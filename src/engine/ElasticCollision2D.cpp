#include "ElasticCollision2D.h"

void trippin::ElasticCollision2D::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides) || rationalVerticalCollision(obj, p, sides)) {
        auto pair = collide(obj.velocity, p.velocity, obj.center, p.center, obj.mass, p.mass);
        obj.velocity = pair.first;
        p.velocity = pair.second;
    }
}

std::pair<trippin::Point<double>, trippin::Point<double>> trippin::ElasticCollision2D::collide(
        trippin::Point<double> v1,
        trippin::Point<double> v2,
        trippin::Point<double> p1,
        trippin::Point<double> p2,
        double m1,
        double m2) {
    auto v1p = collideLeft(v1, v2, p1, p2, m1, m2);
    auto v2p = collideLeft(v2, v1, p2, p1, m2, m1);
    return {v1p, v2p};
}

trippin::Point<double> trippin::ElasticCollision2D::collideLeft(
        trippin::Point<double> v1,
        trippin::Point<double> v2,
        trippin::Point<double> p1,
        trippin::Point<double> p2,
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
    return v1p;
}