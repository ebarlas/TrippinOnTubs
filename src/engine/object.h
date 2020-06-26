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

    void Object::setId(int n) {
        id = n;
    }

    void Object::setPlatform(bool p) {
        platform = p;
    }

    void Object::setSize(const Vector<int> &s) {
        size = s;
    }

    void Object::setPosition(const Vector<double> &p) {
        position = p;
        updateRounded();
    }

    void Object::setVelocity(const Vector<double> &v) {
        velocity = v;
    }

    void Object::setTerminalVelocity(const Vector<double> &v) {
        terminalVelocity = v;
    }

    void Object::setAcceleration(const Vector<double> &a) {
        acceleration = a;
    }

    void Object::setMass(double m) {
        mass = m;
    }

    const Vector<double> &Object::getPosition() const {
        return position;
    }

    const Vector<double> &Object::getVelocity() const {
        return velocity;
    }

    const Rect<int> &Object::getRoundedBox() const {
        return roundedBox;
    }

    void Object::updateRounded() {
        center = {position.x + size.x / 2.0, position.y + size.y / 2.0};
        roundedPosition = {static_cast<int>(std::round(position.x)), static_cast<int>(std::round(position.y))};
        roundedCenter = {roundedPosition.x + size.x / 2, roundedPosition.y + size.y / 2};
        roundedBox = {roundedPosition.x, roundedPosition.y, size.x, size.y};
    }

    void Object::applyMotion() {
        if (platformCollisions.testBottom() || platformCollisions.testTop()) {
            velocity.x = velocity.x > 0
                         ? std::max(velocity.x - friction.x, 0.0)
                         : std::min(velocity.x + friction.x, 0.0);
        }
        if (platformCollisions.testLeft() || platformCollisions.testRight()) {
            velocity.y = velocity.y > 0
                         ? std::max(velocity.y - friction.y, 0.0)
                         : std::min(velocity.y + friction.y, 0.0);
        }

        if ((platformCollisions.testLeft() && velocity.x < 0) || (platformCollisions.testRight() && velocity.x > 0)) {
            velocity.x = 0;
        }
        if ((platformCollisions.testTop() && velocity.y < 0) || (platformCollisions.testBottom() && velocity.y > 0)) {
            velocity.y = 0;
        }

        if ((!platformCollisions.testLeft() && gravity.x < 0) ||
            (!platformCollisions.testRight() && gravity.x > 0)) {
            velocity.x = std::max(std::min(velocity.x + gravity.x, terminalVelocity.x), -terminalVelocity.x);
        }
        if ((!platformCollisions.testTop() && gravity.y < 0) ||
            (!platformCollisions.testBottom() && gravity.y > 0)) {
            velocity.y = std::max(std::min(velocity.y + gravity.y, terminalVelocity.y), -terminalVelocity.y);
        }

        if ((!platformCollisions.testLeft() && acceleration.x < 0) ||
            (!platformCollisions.testRight() && acceleration.x > 0)) {
            velocity.x = std::max(std::min(velocity.x + acceleration.x, terminalVelocity.x), -terminalVelocity.x);
        }
        if ((!platformCollisions.testTop() && acceleration.y < 0) ||
            (!platformCollisions.testBottom() && acceleration.y > 0)) {
            velocity.y = std::max(std::min(velocity.y + acceleration.y, terminalVelocity.y), -terminalVelocity.y);
        }

        position += velocity;
        updateRounded();
        platformCollisions = {};
    }

    void Object::snapTo(const Object &other, const Rect<int> &overlap, const Sides &previousCollisions) {
        auto x = 0.0;
        auto y = 0.0;

        // examine horizontal overlap
        if (roundedBox.leftAlignedWith(overlap)) {
            x = overlap.w;
        } else if (roundedBox.rightAlignedWith(overlap)) {
            x = -overlap.w;
        }

        // examine vertical overlap
        if (roundedBox.topAlignedWith(overlap)) {
            y = overlap.h;
        } else if (roundedBox.bottomAlignedWith(overlap)) {
            y = -overlap.h;
        }

        bool updated = false;

        // use smallest displacement
        if (x != 0 && std::abs(x) < std::abs(y)) {
            // dont snap back over previous overlap - oldest snap wins
            if ((x > 0 && !previousCollisions.testRight()) || (x < 0 && !previousCollisions.testLeft())) {
                position.x += x;
                updated = true;
            }
        } else {
            // same as above
            if ((y > 0 && !previousCollisions.testBottom()) || (y < 0 && !previousCollisions.testTop())) {
                position.y += y;
                updated = true;
            }
        }

        if (updated) {
            updateRounded();
        }
    }

    void Object::snapObjects(std::vector<Object *> &objects) {
        auto compare = [](const std::pair<Object *, Sides> &left, const std::pair<Object *, Sides> &right) {
            // 1. platforms are highest priority
            if (left.first->platform)
                return false;
            if (right.first->platform)
                return true;

            // 2. non-platforms with most collision sides is next highest priority
            auto lcnt = left.second.count();
            auto rcnt = right.second.count();
            if (lcnt != rcnt)
                return lcnt < rcnt;

            // 3. non-platforms with highest y position (lower on screen) is next highest priority
            return left.first->position.y < right.first->position.y;
        };

        Queue<Object *, Sides, decltype(compare)> q(compare);

        // 1. initialize queue with all objects
        for (auto obj : objects) {
            q.push(obj);
        }

        // 2. traverse objects in priority order where priority is determined by platform contacts
        while (!q.empty()) {
            auto plat = q.pop();
            for (auto kin : objects) {
                if (plat != kin && !kin->platform) {
                    auto overlap = kin->roundedBox.intersect(plat->roundedBox);
                    if (overlap) {
                        kin->snapTo(*plat, overlap, q.material(kin));
                    }
                    auto collision = kin->roundedBox.collision(plat->roundedBox);
                    if (collision) {
                        q.push(kin, collision);
                    }
                }
            }
        }
    }

    void Object::reflectiveCollision(Object &p, const Sides &collision) {
        if (collision.testLeft() || collision.testRight()) {
            velocity.x *= -1;
        }
        if (collision.testTop() || collision.testBottom()) {
            velocity.y *= -1;
        }
    }

    void Object::absorbantCollision(Object &p, const Sides &collision) {
        platformCollisions |= collision;
    }

    void Object::elasticCollision1D(Object &p, const Sides &collision) {
        // ignore irrational collisions due to snapping
        // we're interested in applying snapped collisions due, at least in part, to velocities
        // for example, ignore a top collision were k1 has no velocity and k2 has a negative y velocity
        if ((collision.testLeft() && (velocity.x < 0 || p.velocity.x > 0)) ||
            (collision.testRight() && (velocity.x > 0 || p.velocity.x < 0))) {
            auto vels = trippin::elasticCollision1D(velocity.x, p.velocity.x, mass, p.mass);
            velocity.x = vels.first;
            p.velocity.x = vels.second;
        }
        if ((collision.testTop() && (velocity.y < 0 || p.velocity.y > 0)) ||
            (collision.testBottom() && (velocity.y > 0 || p.velocity.y < 0))) {
            auto vels = trippin::elasticCollision1D(velocity.y, p.velocity.y, mass, p.mass);
            velocity.y = vels.first;
            p.velocity.y = vels.second;
        }
    }

    void Object::elasticCollision2D(Object &p, const Sides &collision) {
        if ((collision.testLeft() && (velocity.x < 0 || p.velocity.x > 0)) ||
            (collision.testRight() && (velocity.x > 0 || p.velocity.x < 0)) ||
            ((collision.testTop() && (velocity.y < 0 || p.velocity.y > 0)) ||
             (collision.testBottom() && (velocity.y > 0 || p.velocity.y < 0)))) {
            auto pair = trippin::elasticCollision2D(velocity, p.velocity, center, p.center, mass, p.mass);
            velocity = pair.first;
            p.velocity = pair.second;
        }
    }

    void Object::inelasticCollision2D(Object &p, const Sides &collision) {
        if ((collision.testLeft() && (velocity.x < 0 || p.velocity.x > 0)) ||
            (collision.testRight() && (velocity.x > 0 || p.velocity.x < 0)) ||
            ((collision.testTop() && (velocity.y < 0 || p.velocity.y > 0)) ||
             (collision.testBottom() && (velocity.y > 0 || p.velocity.y < 0)))) {
            auto pair = trippin::inelasticCollision2D(velocity, p.velocity, center, p.center, mass, p.mass, 0.9);
            velocity = pair.first;
            p.velocity = pair.second;
        }
    }

    void Object::applyPhysics(
            std::vector<Object *> &objects,
            const std::function<void(Object &, Object &, const Sides &)> &objPlatFunc,
            const std::function<void(Object &, Object &, const Sides &)> &objObjFunc) {
        for (int i = 0; i < objects.size(); i++) {
            auto a = objects[i];
            for (int j = i + 1; j < objects.size(); j++) {
                auto b = objects[j];
                if (!a->platform || !b->platform) {
                    auto collision = a->roundedBox.collision(b->roundedBox);
                    if (collision) {
                        if (a->platform) {
                            objPlatFunc(*b, *a, collision);
                        } else if (b->platform) {
                            objPlatFunc(*a, *b, collision);
                        } else {
                            objObjFunc(*a, *b, collision);
                        }
                    }
                }
            }
        }
    }

    void Object::applyMotion(std::vector<Object *> &objects) {
        for (auto obj : objects) {
            obj->applyMotion();
        }
    }
}

#endif