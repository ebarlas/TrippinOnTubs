#include "ElasticCollision1D.h"

void trippin::ElasticCollision1D::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides)) {
        auto vels = collide(obj.velocity.x, p.velocity.x, obj.mass, p.mass);
        obj.velocity.x = vels.first;
        p.velocity.x = vels.second;
    }
    if (rationalVerticalCollision(obj, p, sides)) {
        auto vels = collide(obj.velocity.y, p.velocity.y, obj.mass, p.mass);
        obj.velocity.y = vels.first;
        p.velocity.y = vels.second;
    }
}

std::pair<double, double> trippin::ElasticCollision1D::collide(double v1, double v2, double m1, double m2) {
    auto v1p = (m1 - m2) / (m1 + m2) * v1 + (2 * m2) / (m1 + m2) * v2;
    auto v2p = (2 * m1) / (m1 + m2) * v1 + (m2 - m1) / (m1 + m2) * v2;
    return {v1p, v2p};
}
