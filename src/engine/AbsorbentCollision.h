#ifndef TRIPPIN_ABSORBENTCOLLISION_H
#define TRIPPIN_ABSORBENTCOLLISION_H

#include "PlatformCollision.h"

namespace trippin {
    // Collision in which non-platform object loses all velocity in the direction of collision.
    class AbsorbentCollision : public PlatformCollision {
    public:
        void onCollision(Object &left, Object &right, const Sides &sides) override;
    };
}

#endif