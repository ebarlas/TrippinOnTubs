#ifndef TRIPPIN_REFLECTIVECOLLISION_H
#define TRIPPIN_REFLECTIVECOLLISION_H

#include "PlatformCollision.h"

namespace trippin {
    // Collision in which non-platform object is reflected in the direction of collision.
    class ReflectiveCollision : public PlatformCollision {
    public:
        void setCoefficient(double coefficient);
        void onCollision(Object &left, Object &right, const Sides &sides) override;
    private:
        double coefficient = 1.0;
    };
}

#endif