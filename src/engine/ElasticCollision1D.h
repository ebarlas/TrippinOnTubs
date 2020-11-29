#ifndef TRIPPIN_ELASTICCOLLISION1D_H
#define TRIPPIN_ELASTICCOLLISION1D_H

#include "ObjectCollision.h"

namespace trippin {
    // Elastic collision that occurs in only one dimension. Kinetic energy and momentum are conserved.
    class ElasticCollision1D : public ObjectCollision {
    public:
        void onCollision(Object &left, Object &right, const Sides &sides) override;
    private:
        static std::pair<double, double> collide(double v1, double v2, double m1, double m2);
    };
}

#endif