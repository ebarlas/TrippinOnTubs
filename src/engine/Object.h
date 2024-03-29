#ifndef TRIPPIN_OBJECT_H
#define TRIPPIN_OBJECT_H

#include <functional>
#include "engine/Listener.h"
#include "engine/Rect.h"
#include "engine/Point.h"
#include "engine/Sides.h"
#include "engine/Optional.h"

namespace trippin {
    class Object : public Listener {
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

        // The rounded bounding box of the object.
        // This is a meta property based on position and size.
        Rect<int> roundedBox{};

        // Optional platform collision, which overrides engine default.
        // Optional platform collision, which overrides engine default.
        Optional<std::function<void(Object&, Object&, const Sides&)>> platformCollision;
        Optional<std::function<void(Object&, Object&, const Sides&)>> objectCollision;

        // Objects with a non-zero lane interact with objects in the same lane and objects with a zero lane
        // Objects with a zero lane interact with all objects
        // Lane -1 indicates platform contacts only
        // Lane -2 indicates no contacts
        int lane{};

        // Flag that indicates whether this object has expired and can be removed.
        bool expired{};

        // Flag that indicates whether this object should be not considered for movement and collisions.
        bool inactive{};

        // Flag used for object snapping prioritization.
        bool snappedToMe{};

        // Accumulates collision sides over the course of a single round of object snapping.
        // Used to determine object snapping priority.
        Sides snapCollisions{};

        // Collision sides use to determine whether movement is allowed in each direction.
        Sides platformCollisions{};
        Sides objectCollisions{};

        void syncPositions();
        void applyMotion();
        virtual void onPlatformCollision(Object &other, const trippin::Sides &collision);
        virtual void onObjectCollision(Object &other, const trippin::Sides &collision);
        bool isExpired() override;
    };
}

#endif