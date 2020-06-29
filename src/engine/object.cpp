#include "Rect.h"
#include "Vector.h"
#include "Sides.h"
#include "Queue.h"
#include "Object.h"

void trippin::Object::setId(int n) {
    id = n;
}

void trippin::Object::setPlatform(bool p) {
    platform = p;
}

void trippin::Object::setSize(const trippin::Vector<int> &s) {
    size = s;
}

void trippin::Object::setPosition(const trippin::Vector<double> &p) {
    position = p;
    updateRounded();
}

void trippin::Object::setVelocity(const trippin::Vector<double> &v) {
    velocity = v;
}

void trippin::Object::setTerminalVelocity(const trippin::Vector<double> &v) {
    terminalVelocity = v;
}

void trippin::Object::setAcceleration(const trippin::Vector<double> &a) {
    acceleration = a;
}

void trippin::Object::setMass(double m) {
    mass = m;
}

const trippin::Vector<double> &trippin::Object::getPosition() const {
    return position;
}

const trippin::Vector<double> &trippin::Object::getVelocity() const {
    return velocity;
}

const trippin::Vector<double> &trippin::Object::getTerminalVelocity() const {
    return terminalVelocity;
}

const trippin::Vector<double> &trippin::Object::getGravity() const {
    return gravity;
}

const trippin::Vector<double> &trippin::Object::getFriction() const {
    return friction;
}

const trippin::Rect<int> &trippin::Object::getRoundedBox() const {
    return roundedBox;
}

void trippin::Object::updateRounded() {
    center = {position.x + size.x / 2.0, position.y + size.y / 2.0};
    roundedPosition = {static_cast<int>(std::round(position.x)), static_cast<int>(std::round(position.y))};
    roundedCenter = {roundedPosition.x + size.x / 2, roundedPosition.y + size.y / 2};
    roundedBox = {roundedPosition.x, roundedPosition.y, size.x, size.y};
}

void trippin::Object::applyMotion() {
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
    objectCollisions = {};
}

const trippin::Vector<int> &trippin::Object::getSize() const {
    return size;
}

void trippin::Object::setFriction(const Vector<double> &f) {
    friction = f;
}

void trippin::Object::setGravity(const Vector<double> &g) {
    gravity = g;
}

const trippin::Sides &trippin::Object::getPlatformCollisions() const {
    return platformCollisions;
}

const trippin::Vector<int> &trippin::Object::getRoundedPosition() const {
    return roundedPosition;
}

const trippin::Vector<double> &trippin::Object::getCenter() const {
    return center;
}

const trippin::Vector<int> &trippin::Object::getRoundedCenter() const {
    return roundedCenter;
}

const trippin::Vector<double> &trippin::Object::getAcceleration() const {
    return acceleration;
}

const trippin::Sides &trippin::Object::getObjectCollisions() const {
    return objectCollisions;
}

void trippin::Object::onPlatformCollision(trippin::Object &other, const trippin::Sides &collision) {

}

void trippin::Object::onObjectCollision(trippin::Object &other, const trippin::Sides &collision) {

}

int trippin::Object::getId() const {
    return id;
}
