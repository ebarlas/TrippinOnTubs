#include "InelasticCollision.h"
#include "PlasmaPhysics.h"

void trippin::InelasticCollision::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides) || rationalVerticalCollision(obj, p, sides)) {
        auto pair = collide(obj.velocity, p.velocity, obj.center, p.center, obj.mass, p.mass, restitutionCoefficient);
        obj.velocity = pair.first;
        p.velocity = pair.second;
    }
}

void trippin::InelasticCollision::setRestitutionCoefficient(double r) {
    restitutionCoefficient = r;
}

std::pair<trippin::Point<double>, trippin::Point<double>> trippin::InelasticCollision::collide(
        trippin::Point<double> v1,
        trippin::Point<double> v2,
        trippin::Point<double> p1,
        trippin::Point<double> p2,
        double m1,
        double m2,
        double restitutionCoefficient) {
    collision2Ds(m1, m2, restitutionCoefficient, p1.x, p1.y, p2.x, p2.y, v1.x, v1.y, v2.x, v2.y);
    return {v1, v2};
}
