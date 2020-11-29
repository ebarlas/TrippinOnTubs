#include "PlatformCollision.h"

bool trippin::PlatformCollision::rationalHorizontalCollision(Object &obj, Object &p, const Sides &sides) {
    return (sides.testLeft() && obj.velocity.x < 0)
           || (sides.testRight() && obj.velocity.x > 0);
}

bool trippin::PlatformCollision::rationalVerticalCollision(Object &obj, Object &p, const Sides &sides) {
    return (sides.testTop() && obj.velocity.y < 0)
           || (sides.testBottom() && obj.velocity.y > 0);
}
