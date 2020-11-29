#ifndef TRIPPIN_INELASTICCOLLISION_H
#define TRIPPIN_INELASTICCOLLISION_H

#include "ObjectCollision.h"

namespace trippin {
    // Inelastic collision that occurs in two dimensions.
    // The line of collision is drawn between the object center points.
    // The coefficient of restitution is configurable.
    // Momentum is conserved but kinetic energy is not conserved.
    class InelasticCollision : public ObjectCollision {
    public:
        void onCollision(Object &left, Object &right, const Sides &sides) override;
        void setRestitutionCoefficient(double r);
    private:
        double restitutionCoefficient = 0.9;
        static std::pair<Point<double>, Point<double>> collide(
                Point<double> v1,
                Point<double> v2,
                Point<double> p1,
                Point<double> p2,
                double m1,
                double m2,
                double restitutionCoefficient);
    };
}

#endif