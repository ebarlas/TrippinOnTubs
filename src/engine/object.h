#ifndef TRIPPIN_OBJECT_H
#define TRIPPIN_OBJECT_H

#include <iostream>
#include <functional>
#include "rect.h"
#include "vector.h"
#include "sides.h"
#include "queue.h"
#include "physics.h"

namespace trippin {
    class Object {
    protected:
        int id{};
        bool platform{};
        double mass{};
        Vector<double> position{};
        Vector<double> center{};
        Vector<double> velocity{};
        Vector<double> acceleration{};
        Vector<double> terminalVelocity{};
        Vector<double> friction{};
        Vector<double> gravity{};
        Vector<int> size{};

        Vector<int> roundedPosition{};
        Vector<int> roundedCenter{};
        Rect<int> roundedBox{};

        Sides platformCollisions{};

        void updateRounded();
    public:
        void setId(int id);
        void setPlatform(bool p);
        void setSize(const Vector<int> &s);
        void setPosition(const Vector<double> &p);
        void setVelocity(const Vector<double> &v);
        void setTerminalVelocity(const Vector<double> &v);
        void setAcceleration(const Vector<double> &a);
        void setMass(double m);
        const Vector<double> &getPosition() const;
        const Vector<double> &getVelocity() const;
        const Rect<int> &getRoundedBox() const;
        void applyMotion();
        static void applyMotion(std::vector<Object *> &objects);
        void snapTo(const Object &other, const Rect<int> &overlap, const Sides &sides);
        static void snapObjects(std::vector<Object *> &objects);
        static void applyPhysics(
                std::vector<Object *> &objects,
                const std::function<void(Object &, Object &, const Sides &)> &objPlatFunc,
                const std::function<void(Object &, Object &, const Sides &)> &objObjFunc);
        void reflectiveCollision(Object &p, const Sides &collision);
        void absorbantCollision(Object &p, const Sides &collision);
        void elasticCollision1D(Object &p, const Sides &collision);
        void elasticCollision2D(Object &p, const Sides &collision);
        void inelasticCollision2D(Object &p, const Sides &collision);
    };
}

#endif