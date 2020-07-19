#ifndef TRIPPIN_OBJECT_H
#define TRIPPIN_OBJECT_H

#include <vector>
#include "engine/Rect.h"
#include "engine/Point.h"
#include "engine/Sides.h"
#include "engine/Optional.h"
#include "engine/CollisionType.h"

namespace trippin {
    class Partition;

    class Object {
        friend class Engine;
        friend class Grid;
        friend class Partition;

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

        std::vector<Partition *> partitions;

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
        Point<double> getPosition() const;
        Point<double> getCenter() const;
        Point<double> getVelocity() const;
        Point<double> getAcceleration() const;
        Point<double> getGravity() const;
        Point<double> getFriction() const;
        Point<double> getTerminalVelocity() const;
        Point<int> getSize() const;
        Rect<int> getRoundedBox() const;
        Point<int> getRoundedPosition() const;
        Point<int> getRoundedCenter() const;
        Sides getPlatformCollisions() const;
        Sides getObjectCollisions() const;
        void applyMotion();
        virtual void onPlatformCollision(Object &other, const trippin::Sides &collision);
        virtual void onObjectCollision(Object &other, const trippin::Sides &collision);
    };
}

#endif