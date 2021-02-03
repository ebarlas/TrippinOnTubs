#include "ReflectiveCollision.h"

void trippin::ReflectiveCollision::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides)) {
        obj.velocity.x *= -1 * coefficient;
    }
    if (rationalVerticalCollision(obj, p, sides)) {
        obj.velocity.y *= -1 * coefficient;
    }
}

void trippin::ReflectiveCollision::setCoefficient(double co) {
    coefficient = co;
}
