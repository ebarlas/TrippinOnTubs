#ifndef TRIPPIN_OBJECTCOLLISION_H
#define TRIPPIN_OBJECTCOLLISION_H

#include "Collision.h"

namespace trippin {
    // Collision types between two non-platforms.
    class ObjectCollision : public Collision {
    protected:
        bool rationalHorizontalCollision(Object &left, Object &right, const Sides &sides) override;
        bool rationalVerticalCollision(Object &left, Object &right, const Sides &sides) override;
    };
}

#endif