#ifndef TRIPPIN_COLLISIONTYPE_H
#define TRIPPIN_COLLISIONTYPE_H

namespace trippin {
    enum class PlatformCollisionType {
        absorbant,
        reflective
    };

    enum class ObjectCollisionType {
        elastic1D,
        elastic2D,
        inelastic
    };
}

#endif
