#ifndef TRIPPIN_OBJECT_H
#define TRIPPIN_OBJECT_H

#include <iostream>
#include <functional>
#include "Rect.h"
#include "Vector.h"
#include "Sides.h"
#include "Queue.h"
#include "Physics.h"

namespace trippin {
    class Object {
        friend class Engine;

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
        void setFriction(const Vector<double> &f);
        void setGravity(const Vector<double> &g);
        const Vector<double> &getPosition() const;
        const Vector<double> &getVelocity() const;
        const Vector<double> &getGravity() const;
        const Vector<double> &getFriction() const;
        const Vector<double> &getTerminalVelocity() const;
        const Vector<int> &getSize() const;
        const Rect<int> &getRoundedBox() const;
        const Vector<int> &getRoundedPosition() const;
        const Sides &getPlatformCollisions() const;
        void applyMotion();
    };
}

#endif