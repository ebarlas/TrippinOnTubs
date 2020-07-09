#ifndef TRIPPIN_PHYSICS_H
#define TRIPPIN_PHYSICS_H

#include <utility>
#include "Point.h"
#include "PlasmaPhysics.h"

namespace trippin {

    std::pair<double, double> elasticCollision1D(
            double v1,
            double v2,
            double m1,
            double m2);

    std::pair<DoublePoint, DoublePoint> elasticCollision2D(
            DoublePoint v1,
            DoublePoint v2,
            DoublePoint p1,
            DoublePoint p2,
            double m1,
            double m2);

    std::pair<DoublePoint, DoublePoint> inelasticCollision2D(
            DoublePoint v1,
            DoublePoint v2,
            DoublePoint p1,
            DoublePoint p2,
            double m1,
            double m2,
            double restitutionCoefficient);

}

#endif