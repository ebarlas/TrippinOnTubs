#ifndef TRIPPIN_OBJECT_H
#define TRIPPIN_OBJECT_H

#include "engine/Rect.h"
#include "engine/Point.h"
#include "engine/Sides.h"
#include "engine/Optional.h"
#include "engine/CollisionType.h"

namespace trippin {
    class Object {
        friend class Engine;

    protected:
        int id{};
        bool platform{};
        double mass{};
        Point<double> position{};
        Point<double> center{};
        Point<double> velocity{};
        Point<double> acceleration{};
        Point<double> terminalVelocity{};
        Point<double> friction{};
        Point<double> gravity{};
        Point<int> size{};

        Point<int> roundedPosition{};
        Point<int> roundedCenter{};
        Rect<int> roundedBox{};

        Optional<PlatformCollisionType> platformCollisionType{};

        Sides platformCollisions{};
        Sides objectCollisions{};

        void updateRounded();
    public:
        void setId(int id);
        void setPlatform(bool p);
        void setSize(const Point<int> &s);
        void setPosition(const Point<double> &p);
        void setVelocity(const Point<double> &v);
        void setTerminalVelocity(const Point<double> &v);
        void setAcceleration(const Point<double> &a);
        void setMass(double m);
        void setFriction(const Point<double> &f);
        void setGravity(const Point<double> &g);
        void setPlatformCollisionType(PlatformCollisionType type);
        int getId() const;
        const Point<double> &getPosition() const;
        const Point<double> &getCenter() const;
        const Point<double> &getVelocity() const;
        const Point<double> &getAcceleration() const;
        const Point<double> &getGravity() const;
        const Point<double> &getFriction() const;
        const Point<double> &getTerminalVelocity() const;
        const Point<int> &getSize() const;
        const Rect<int> &getRoundedBox() const;
        const Point<int> &getRoundedPosition() const;
        const Point<int> &getRoundedCenter() const;
        const Sides &getPlatformCollisions() const;
        const Sides &getObjectCollisions() const;
        void applyMotion();
        virtual void onPlatformCollision(Object &other, const trippin::Sides &collision);
        virtual void onObjectCollision(Object &other, const trippin::Sides &collision);
    };
}

#endif