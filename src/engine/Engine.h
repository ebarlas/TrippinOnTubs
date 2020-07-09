#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <unordered_set>
#include "Object.h"
#include "CollisionType.h"

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
        // (1) apply motion to objects
        // (2) snap object to discrete positions
        // (3) detect and react to collisions
        void tick();
    private:
        struct CollisionKey {
            int id1;
            int id2;
            Point<double> v1;
            Point<double> v2;
        };

        struct CollisionKeyHash {
            int operator()(const CollisionKey &ck) const {
                return static_cast<int>(ck.id1 + ck.id2 + ck.v1.x + ck.v1.y + ck.v2.x + ck.v2.y);
            }
        };

        struct CollisionKeyEq {
            bool operator()(const CollisionKey &left, const CollisionKey &right) const {
                return left.id1 == right.id1 && left.id2 == right.id2 && left.v1 == right.v1 && left.v2 == right.v2;
            }
        };

        std::unordered_set<CollisionKey, CollisionKeyHash, CollisionKeyEq> prevCollisions;
        std::unordered_set<CollisionKey, CollisionKeyHash, CollisionKeyEq> nextCollisions;

        std::vector<Object *> platforms;
        std::vector<Object *> objects;
        PlatformCollisionType platformCollisionType;
        ObjectCollisionType objectCollisionType;

        void transferCollisions();
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
        bool testCollision(const trippin::Object &obj, const trippin::Object &p);
        void registerCollision(const trippin::Object &obj, const trippin::Object &p, const Point<double> &v1,
                               const Point<double> &v2);
        static bool rationalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalHorizontalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalVerticalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
    };
}

#endif
