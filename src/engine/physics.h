#ifndef TRIPPIN_PHYSICS_H
#define TRIPPIN_PHYSICS_H

#include <utility>
#include "vector.h"
#include "plasmaphysics.h"

namespace trippin {

    std::pair<double, double> elasticCollision1D(
            double v1,
            double v2,
            double m1,
            double m2);

    std::pair<DoubleVector, DoubleVector> elasticCollision2D(
            DoubleVector v1,
            DoubleVector v2,
            DoubleVector p1,
            DoubleVector p2,
            double m1,
            double m2);

    std::pair<DoubleVector, DoubleVector> inelasticCollision2D(
            DoubleVector v1,
            DoubleVector v2,
            DoubleVector p1,
            DoubleVector p2,
            double m1,
            double m2,
            double restitutionCoefficient);

}

#endif