#ifndef TRIPPIN_ELASTICCOLLISION2D_H
#define TRIPPIN_ELASTICCOLLISION2D_H

#include "ObjectCollision.h"

namespace trippin {
    // Elastic collision that occurs in two dimensions.
    // The line of collision is drawn between the object center points.
    // Kinetic energy and momentum are conserved.
    class ElasticCollision2D : public ObjectCollision {
    public:
        void onCollision(Object &left, Object &right, const Sides &sides) override;
    private:
        static std::pair<Point<double>, Point<double>> collide(
                Point<double> v1,
                Point<double> v2,
                Point<double> p1,
                Point<double> p2,
                double m1,
                double m2);
        static Point<double> collideLeft(
                Point<double> v1,
                Point<double> v2,
                Point<double> p1,
                Point<double> p2,
                double m1,
                double m2);
    };
}

#endif