#include <cmath>
#include "InelasticCollision.h"

void trippin::InelasticCollision::onCollision(Object &obj, Object &p, const Sides &sides) {
    if (rationalHorizontalCollision(obj, p, sides) || rationalVerticalCollision(obj, p, sides)) {
        auto pair = collide(
                obj.velocity, p.velocity,
                obj.center, p.center,
                obj.mass, p.mass,
                restitutionCoefficient);
        obj.velocity = pair.first;
        p.velocity = pair.second;
    }
}

void trippin::InelasticCollision::setRestitutionCoefficient(double r) {
    restitutionCoefficient = r;
}

// Adapted from code snippet: http://www.plasmaphysics.org.uk/collision2d.htm
std::pair<trippin::Point<double>, trippin::Point<double>> trippin::InelasticCollision::collide(
        trippin::Point<double> v1,
        trippin::Point<double> v2,
        trippin::Point<double> p1,
        trippin::Point<double> p2,
        double m1,
        double m2,
        double restitutionCoefficient) {
    double xDiff = p2.x - p1.x;
    double yDiff = p2.y - p1.y;
    double r = sqrt(xDiff * xDiff + yDiff * yDiff) / 2;

    double r12, m21, d, gammav, gammaxy, dgamma, dr,
            dvx2, a, x21, y21, vx21, vy21, pi2, vx_cm, vy_cm;

    //     ***initialize some variables ****
    pi2 = 2 * acos(-1.0E0);
    r12 = r + r;
    m21 = m2 / m1;
    x21 = xDiff;
    y21 = yDiff;
    vx21 = v2.x - v1.x;
    vy21 = v2.y - v1.y;

    vx_cm = (m1 * v1.x + m2 * v2.x) / (m1 + m2);
    vy_cm = (m1 * v1.y + m2 * v2.y) / (m1 + m2);

    //     ****  return old positions and velocities if relative velocity =0 ****
    if (vx21 == 0 && vy21 == 0)
        return {v1, v2};


    //     *** calculate relative velocity angle
    gammav = atan2(-vy21, -vx21);

    d = sqrt(x21 * x21 + y21 * y21);

    //     **** return if distance between balls smaller than sum of radii ***
    if (d < r12)
        return {v1, v2};

    //     *** calculate relative position angle and normalized impact parameter ***
    gammaxy = atan2(y21, x21);
    dgamma = gammaxy - gammav;
    if (dgamma > pi2) { dgamma = dgamma - pi2; }
    else if (dgamma < -pi2) { dgamma = dgamma + pi2; }
    dr = d * sin(dgamma) / r12;

    //     **** return old positions and velocities if balls do not collide ***
    if ((fabs(dgamma) > pi2 / 4 && fabs(dgamma) < 0.75 * pi2) || fabs(dr) > 1)
        return {v1, v2};


    //     **** calculate impact angle if balls do collide ***
    auto alpha = asin(dr);


    //     **** calculate time to collision ***

    a = tan(gammav + alpha);

    dvx2 = -2 * (vx21 + a * vy21) / ((1 + a * a) * (1 + m21));

    v2.x = v2.x + dvx2;
    v2.y = v2.y + a * dvx2;
    v1.x = v1.x - m21 * dvx2;
    v1.y = v1.y - a * m21 * dvx2;

    //     ***  velocity correction for inelastic collisions ***

    v1.x = (v1.x - vx_cm) * restitutionCoefficient + vx_cm;
    v1.y = (v1.y - vy_cm) * restitutionCoefficient + vy_cm;
    v2.x = (v2.x - vx_cm) * restitutionCoefficient + vx_cm;
    v2.y = (v2.y - vy_cm) * restitutionCoefficient + vy_cm;

    return {v1, v2};
}
