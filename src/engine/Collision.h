#ifndef TRIPPIN_COLLISION_H
#define TRIPPIN_COLLISION_H

#include "Object.h"

namespace trippin {
    class Collision {
    public:
        virtual void onCollision(Object &left, Object &right, const Sides &sides) = 0;
    protected:
        virtual bool rationalHorizontalCollision(Object &left, Object &right, const Sides &sides) = 0;
        virtual bool rationalVerticalCollision(Object &left, Object &right, const Sides &sides) = 0;
    };
}

#endif