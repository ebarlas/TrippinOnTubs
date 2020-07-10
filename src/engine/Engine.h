#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <unordered_set>
#include "Object.h"
#include "CollisionType.h"
#include "CollisionTable.h"

namespace trippin {
    // Engine handles the movement and interaction of objects.
    class Engine {
    public:
        // Add an object to the engine. The is a weak reference.
        // The object ought to out-live the engine.
        void add(Object *object);

        // Sets the default type of collision between platform and non-platform objects.
        // This collision type can be overridden by individual objects.
        void setPlatformCollisionType(PlatformCollisionType t);

        // Sets the default type of collision between objects.
        // This collision type can be overridden by individual objects.
        void setObjectCollisionType(ObjectCollisionType t);

        // Advance the engine simulation by one tick. In each tick:
        // (1) apply motion to objects and snap to grid
        // (2) detect intersections and snap objects
        // (3) detect and react to collisions
        void tick();
    private:
        std::vector<Object *> platforms;
        std::vector<Object *> objects;
        PlatformCollisionType platformCollisionType;
        ObjectCollisionType objectCollisionType;
        CollisionTable collisionTable;

        void applyMotion();
        void snapObjects();
        void applyPhysics();
        void snapTo(Object &obj, const Object &p, const trippin::Rect<int> &overlap, const Sides &previousContacts);

        void applyPlatformCollision(Object &object, Object &platform, const Sides &collision);
        void applyObjectCollision(Object &left, Object &right, const Sides &collision);
        void absorbantCollision(Object &object, Object &platform, const Sides &collision);
        void reflectiveCollision(Object &object, Object &platform, const Sides &collision);
        void elasticCollision1D(Object &object, Object &platform, const Sides &collision);
        void elasticCollision2D(Object &object, Object &platform, const Sides &collision);
        void inelasticCollision2D(Object &object, Object &platform, const Sides &collision);

        static bool rationalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalHorizontalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalVerticalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
    };
}

#endif
