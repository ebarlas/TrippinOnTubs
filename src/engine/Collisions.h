#ifndef TRIPPIN_COLLISIONS_H
#define TRIPPIN_COLLISIONS_H

#include "Object.h"
#include "PlasmaPhysics.h"

namespace trippin {
    inline bool rationalHorizontalCollision(Object &obj, Object &p, const Sides &sides) {
        auto &ov = obj.velocity;
        auto &pv = p.velocity;
        auto leftCollision = sides.testLeft() && ((ov.x < 0 && ov.x < pv.x) || (pv.x > 0 && pv.x > ov.x));
        auto rightCollision = sides.testRight() && ((ov.x > 0 && ov.x > pv.x) || (pv.x < 0 && pv.x < ov.x));
        return leftCollision || rightCollision;
    }

    inline bool rationalVerticalCollision(Object &obj, Object &p, const Sides &sides) {
        auto &ov = obj.velocity;
        auto &pv = p.velocity;
        auto topCollision = sides.testTop() && ((ov.y < 0 && ov.y < pv.y) || (pv.y > 0 && pv.y > ov.y));
        auto bottomCollision = sides.testBottom() && ((ov.y > 0 && ov.y > pv.y) || (pv.y < 0 && pv.y < ov.y));
        return topCollision || bottomCollision;
    }

    inline std::pair<Point<double>, Point<double>> collide(
            Point<double> v1,
            Point<double> v2,
            Point<double> p1,
            Point<double> p2,
            double m1,
            double m2,
            double restitutionCoefficient) {
        collision2Ds(m1, m2, restitutionCoefficient, p1.x, p1.y, p2.x, p2.y, v1.x, v1.y, v2.x, v2.y);
        return {v1, v2};
    }

    inline void onInelasticCollision(Object &obj, Object &p, const Sides &sides, double restitutionCoefficient) {
        if (rationalHorizontalCollision(obj, p, sides) || rationalVerticalCollision(obj, p, sides)) {
            auto pair = collide(obj.velocity, p.velocity, obj.center, p.center, obj.mass, p.mass, restitutionCoefficient);
            obj.velocity = pair.first;
            p.velocity = pair.second;
        }
    }

    inline void onInelasticCollisionDefault(Object &obj, Object &p, const Sides &sides) {
        onInelasticCollision(obj, p, sides, 0.9);
    }

    inline void onElasticCollision2D(Object &obj, Object &p, const Sides &sides) {
        onInelasticCollision(obj, p, sides, 1.0);
    }

    inline std::pair<double, double> collide(double v1, double v2, double m1, double m2) {
        auto v1p = (m1 - m2) / (m1 + m2) * v1 + (2 * m2) / (m1 + m2) * v2;
        auto v2p = (2 * m1) / (m1 + m2) * v1 + (m2 - m1) / (m1 + m2) * v2;
        return {v1p, v2p};
    }

    inline void onElasticCollision1D(Object &obj, Object &p, const Sides &sides) {
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

    inline void onAbsorbentCollision(Object &obj, Object &p, const Sides &sides) {
        if (rationalHorizontalCollision(obj, p, sides)) {
            obj.velocity.x = 0;
        }
        if (rationalVerticalCollision(obj, p, sides)) {
            obj.velocity.y = 0;
        }
        obj.platformCollisions |= sides;
    }

    inline void onReflectiveCollision(Object &obj, Object &p, const Sides &sides, double coefficient) {
        if (rationalHorizontalCollision(obj, p, sides)) {
            obj.velocity.x *= -1 * coefficient;
        }
        if (rationalVerticalCollision(obj, p, sides)) {
            obj.velocity.y *= -1 * coefficient;
        }
    }

    inline void onReflectiveCollisionDefault(Object &obj, Object &p, const Sides &sides) {
        onReflectiveCollision(obj, p, sides, 1.0);
    }
}

#endif