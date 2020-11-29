#ifndef TRIPPIN_PLATFORMCOLLISION_H
#define TRIPPIN_PLATFORMCOLLISION_H

#include "Collision.h"

namespace trippin {
    // Collision between platforms and non-platforms.
    class PlatformCollision : public Collision {
    protected:
        bool rationalHorizontalCollision(Object &left, Object &right, const Sides &sides) override;
        bool rationalVerticalCollision(Object &left, Object &right, const Sides &sides) override;
    };
}

#endif