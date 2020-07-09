#ifndef TRIPPIN_COLLISIONTYPE_H
#define TRIPPIN_COLLISIONTYPE_H

namespace trippin {
    // Collision types between platforms and non-platforms.
    enum class PlatformCollisionType {
        // Collision in which non-platform object loses all velocity in the direction of collision.
        absorbant,

        // Collision in which non-platform object is reflected in the direction of collision.
        reflective
    };

    // Collision types between two non-platforms.
    enum class ObjectCollisionType {
        // Elastic collision that occurs in only one dimension. Kinetic energy and momentum are conserved.
        elastic1D,

        // Elastic collision that occurs in two dimensions.
        // The line of collision is drawn between the object center points.
        // Kinetic energy and momentum are conserved.
        elastic2D,

        // Inelastic collision that occurs in two dimensions.
        // The line of collision is drawn between the object center points.
        // The coefficient of restitution is configurable.
        // Momentum is conserved but kinetic energy is not conserved.
        inelastic
    };
}

#endif
