#include <cmath>
#include "PlasmaPhysics.h"

//******************************************************************************
//   This program is a 'remote' 2D-collision detector for two balls on linear
//   trajectories and returns, if applicable, the location of the collision for
//   both balls as well as the new velocity vectors (assuming a partially elastic
//   collision as defined by the restitution coefficient).
//   The equations on which the code is based have been derived at
//   http://www.plasmaphysics.org.uk/collision2d.htm
//
//   In  'f' (free) mode no positions but only the initial velocities
//   and an impact angle are required.
//   All variables apart from 'mode' and 'error' are of Double Precision
//   Floating Point type.
//
//   The Parameters are:
//
//    mode  (char) (if='f' alpha must be supplied; otherwise arbitrary)
//    alpha (impact angle) only required in mode='f';
//                     should be between -PI/2 and PI/2 (0 = head-on collision))
//    R    (restitution coefficient)  between 0 and 1 (1=perfectly elastic collision)
//    m1   (mass of ball 1)
//    m2   (mass of ball 2)
//    r1   (radius of ball 1)        not needed for 'f' mode
//    r2   (radius of ball 2)                "
//  & x1   (x-coordinate of ball 1)          "
//  & y1   (y-coordinate of ball 1)          "
//  & x2   (x-coordinate of ball 2)          "
//  & y2   (y-coordinate of ball 2)          "
//  & vx1  (velocity x-component of ball 1)
//  & vy1  (velocity y-component of ball 1)
//  & vx2  (velocity x-component of ball 2)
//  & vy2  (velocity y-component of ball 2)
//  & error (int)  (0: no error
//                  1: balls do not collide
//                  2: initial positions impossible (balls overlap))
//
//   Note that the parameters with an ampersand (&) are passed by reference,
//   i.e. the corresponding arguments in the calling program will be updated;
//   however, the coordinates and velocities will only be updated if 'error'=0.
//
//   All variables should have the same data types in the calling program
//   and all should be initialized before calling the function even if
//   not required in the particular mode.
//
//   This program is free to use for everybody. However, you use it at your own
//   risk and I do not accept any liability resulting from incorrect behaviour.
//   I have tested the program for numerous cases and I could not see anything
//   wrong with it but I can not guarantee that it is bug-free under any
//   circumstances.
//
//   I would appreciate if you could report any problems to me
//   (for contact details see  http://www.plasmaphysics.org.uk/feedback.htm ).
//
//   Thomas Smid, January  2004
//                December 2005 (corrected faulty collision detection;
//                               a few minor changes to improve speed;
//                               added simplified code without collision detection)
//                December 2009 (generalization to partially inelastic collisions)
//*********************************************************************************
//void collision2D(char mode, double alpha, double R,
//                 double m1, double m2, double r1, double r2,
//                 double &x1, double &y1, double &x2, double &y2,
//                 double &vx1, double &vy1, double &vx2, double &vy2,
//                 int &error) {
//    ...
//}


//******************************************************************************
//  Simplified Version
//  The advantage of the 'remote' collision detection in the program above is
//  that one does not have to continuously track the balls to detect a collision.
//  The program needs only to be called once for any two balls unless their
//  velocity changes. However, if somebody wants to use a separate collision
//  detection routine for whatever reason, below is a simplified version of the
//  code which just calculates the new velocities, assuming the balls are already
//  touching (this condition is important as otherwise the results will be incorrect)
//****************************************************************************
void trippin::collision2Ds(
        int_fast64_t m1,
        int_fast64_t m2,
        Fraction<int_fast64_t> R,
        int_fast64_t x1,
        int_fast64_t y1,
        int_fast64_t x2,
        int_fast64_t y2,
        int_fast64_t &vx1,
        int_fast64_t &vy1,
        int_fast64_t &vx2,
        int_fast64_t &vy2) {

    int_fast64_t m21, dvx2, a, x21, y21, vx21, vy21, fy21, sign, vx_cm, vy_cm;


    m21 = m2 / m1;
    x21 = x2 - x1;
    y21 = y2 - y1;
    vx21 = vx2 - vx1;
    vy21 = vy2 - vy1;

    vx_cm = (m1 * vx1 + m2 * vx2) / (m1 + m2);
    vy_cm = (m1 * vy1 + m2 * vy2) / (m1 + m2);


    //     *** return old velocities if balls are not approaching ***
    if ((vx21 * x21 + vy21 * y21) >= 0) return;


    //     *** I have inserted the following statements to avoid a zero divide;
    //         (for single precision calculations,
    //          1.0E-12 should be replaced by a larger value). **************

    /*
    fy21 = 1.0E-12 * fabs(y21);
    if (fabs(x21) < fy21) {
        if (x21 < 0) { sign = -1; } else { sign = 1; }
        x21 = fy21 * sign;
    }
    */

    //     ***  update velocities ***
    a = y21 / x21;
    dvx2 = -2 * (vx21 + a * vy21) / ((1 + a * a) * (1 + m21));
    vx2 = vx2 + dvx2;
    vy2 = vy2 + a * dvx2;
    vx1 = vx1 - m21 * dvx2;
    vy1 = vy1 - a * m21 * dvx2;

    //     ***  velocity correction for inelastic collisions ***
    vx1 = static_cast<int_fast64_t>(R * (vx1 - vx_cm)) + vx_cm;
    vy1 = static_cast<int_fast64_t>(R * (vy1 - vy_cm)) + vy_cm;
    vx2 = static_cast<int_fast64_t>(R * (vx2 - vx_cm)) + vx_cm;
    vy2 = static_cast<int_fast64_t>(R * (vy2 - vy_cm)) + vy_cm;
}