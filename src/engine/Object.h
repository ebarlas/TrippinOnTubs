#ifndef TRIPPIN_OBJECT_H
#define TRIPPIN_OBJECT_H

#include <vector>
#include "engine/Rect.h"
#include "engine/Point.h"
#include "engine/Sides.h"
#include "engine/Optional.h"
#include "engine/Clock.h"

namespace trippin {
    class Collision;

    class Object {
    public:
        // A unique identifier for this object.
        int id{};

        // A flag indicating whether this object is a platform.
        // Platforms cannot collide with each other and platforms have the highest priority during object snapping.
        bool platform{};

        // The mass of this objects. This is important for calculating collision reactions.
        double mass{};

        // The top left corner position of the object in abstract units.
        Point<double> position{};

        // The size of the object in abstract units.
        Point<int> size{};

        // The velocity of the object in units per engine tick.
        Point<double> velocity{};

        // The acceleration of the object in units per engine tick per engine tick.
        Point<double> acceleration{};

        // The terminal velocity of the object in units per engine tick.
        // The object cannot exceed the maximum in either dimension.
        Point<double> terminalVelocity{};

        // The deceleration due to friction in units per engine tick per engine tick.
        // This quantity is applied when an object is in contact with an absorbent platform.
        Point<double> friction{};

        // The acceleration due to gravity in units per engine tick per engine tick.
        // This quantity is applied continuously, except when an object is in contact with a platform.
        double gravity{};

        // The acceleration due to gravity applied to falling objects in units per engine tick per engine tick.
        // This quantity is applied continuously, except when an object is in contact with a platform.
        // This is used instead of (not in addition to) the normal gravity.
        // A value of 0 indicated that it is unset.
        double fallGravity{};

        // The rounded top left corner position of the object.
        // This is a meta property based on position.
        Point<int> roundedPosition{};

        // The center position of the object.
        // This is a meta property based on position and size.
        Point<double> center{};

        // The rounded center of the object.
        // This is a meta property based on position and size.
        Point<int> roundedCenter{};

        // The rounded bounding box of the object.
        // This is a meta property based on position and size.
        Rect<int> roundedBox{};

        // Optional platform collision, which overrides engine default.
        Optional<Collision *> platformCollision{};

        // Flag that indicates whether this object has expired and can be removed.
        bool expired{};

        // Flag used by snap priority queue.
        bool queueVisited{};

        // Collision sides use to determine whether movement is allowed in each direction.
        Sides platformCollisions{};
        Sides objectCollisions{};

        // Accumulates collision sides over the course of a single round of object snapping
        Sides snapCollisions{};

        void syncPositions();
        void applyMotion();
        virtual void onPlatformCollision(Object &other, const trippin::Sides &collision);
        virtual void onObjectCollision(Object &other, const trippin::Sides &collision);
        virtual void beforeTick(Uint32 engineTicks);
        virtual void afterTick(Uint32 engineTicks);
    };
}

#endif