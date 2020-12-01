#ifndef TRIPPIN_ELASTICCOLLISION2D_H
#define TRIPPIN_ELASTICCOLLISION2D_H

#include "InelasticCollision.h"

namespace trippin {
    // Elastic collision that occurs in two dimensions.
    // The line of collision is drawn between the object center points.
    // Kinetic energy and momentum are conserved.
    class ElasticCollision2D : public InelasticCollision {
    public:
        ElasticCollision2D();
    };
}

#endif