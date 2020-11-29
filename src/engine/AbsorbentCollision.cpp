#include "AbsorbentCollision.h"

void trippin::AbsorbentCollision::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides)) {
        obj.velocity.x = 0;
    }
    if (rationalVerticalCollision(obj, p, sides)) {
        obj.velocity.y = 0;
    }

    obj.platformCollisions |= sides;
}
