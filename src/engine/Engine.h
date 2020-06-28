#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <unordered_set>
#include "Object.h"

namespace trippin {
    class Engine {
    public:
        enum class PlatformCollisionType {
            absorbant,
            reflective
        };

        enum class ObjectCollisionType {
            elastic1D,
            elastic2D,
            inelastic
        };
    private:
        struct CollisionKey {
            int id1;
            int id2;
            Vector<double> v1;
            Vector<double> v2;
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
        void snapTo(Object &obj, const Object &p, const trippin::Rect<int> &overlap, const Sides &previousCollisions);
        void applyPlatformCollision(Object &object, Object &platform, const Sides &collision);
        void applyObjectCollision(Object &left, Object &right, const Sides &collision);
        void absorbantCollision(Object &object, Object &platform, const Sides &collision);
        void reflectiveCollision(Object &object, Object &platform, const Sides &collision);
        void elasticCollision1D(Object &object, Object &platform, const Sides &collision);
        void elasticCollision2D(Object &object, Object &platform, const Sides &collision);
        void inelasticCollision2D(Object &object, Object &platform, const Sides &collision);
        bool testCollision(const trippin::Object &obj, const trippin::Object &p);
        void registerCollision(const trippin::Object &obj, const trippin::Object &p, const Vector<double> &v1,
                               const Vector<double> &v2);
        static bool rationalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalHorizontalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalVerticalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
    public:
        void add(Object *object);
        void setPlatformCollisionType(PlatformCollisionType t);
        void setObjectCollisionType(ObjectCollisionType t);
        void tick();
    };
}

#endif
