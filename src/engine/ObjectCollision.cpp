#include "ObjectCollision.h"

bool trippin::ObjectCollision::rationalHorizontalCollision(Object &obj, Object &p, const Sides &sides) {
    auto &ov = obj.velocity;
    auto &pv = p.velocity;
    auto leftCollision = sides.testLeft() && ((ov.x < 0 && ov.x < pv.x) || (pv.x > 0 && pv.x > ov.x));
    auto rightCollision = sides.testRight() && ((ov.x > 0 && ov.x > pv.x) || (pv.x < 0 && pv.x < ov.x));
    return leftCollision || rightCollision;
}

bool trippin::ObjectCollision::rationalVerticalCollision(Object &obj, Object &p, const Sides &sides) {
    auto &ov = obj.velocity;
    auto &pv = p.velocity;
    auto topCollision = sides.testTop() && ((ov.y < 0 && ov.y < pv.y) || (pv.y > 0 && pv.y > ov.y));
    auto bottomCollision = sides.testBottom() && ((ov.y > 0 && ov.y > pv.y) || (pv.y < 0 && pv.y < ov.y));
    return topCollision || bottomCollision;
}
