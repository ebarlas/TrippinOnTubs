#include "ReflectiveCollision.h"

void trippin::ReflectiveCollision::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides)) {
        obj.velocity.x *= -1;
    }
    if (rationalVerticalCollision(obj, p, sides)) {
        obj.velocity.y *= -1;
    }
}
