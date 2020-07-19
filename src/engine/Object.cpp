#include <cmath>
#include "engine/Rect.h"
#include "engine/Point.h"
#include "engine/Sides.h"
#include "engine/Object.h"

void trippin::Object::setId(int n) {
    id = n;
}

void trippin::Object::setPlatform(bool p) {
    platform = p;
}

void trippin::Object::setSize(const trippin::Point<int> &s) {
    size = s;
    updateRounded();
}

void trippin::Object::setPosition(const trippin::Point<double> &p) {
    position = p;
    updateRounded();
}

void trippin::Object::setVelocity(const trippin::Point<double> &v) {
    velocity = v;
}

void trippin::Object::setTerminalVelocity(const trippin::Point<double> &v) {
    terminalVelocity = v;
}

void trippin::Object::setAcceleration(const trippin::Point<double> &a) {
    acceleration = a;
}

void trippin::Object::setMass(double m) {
    mass = m;
}

trippin::Point<double> trippin::Object::getPosition() const {
    return position;
}

trippin::Point<double> trippin::Object::getVelocity() const {
    return velocity;
}

trippin::Point<double> trippin::Object::getTerminalVelocity() const {
    return terminalVelocity;
}

trippin::Point<double> trippin::Object::getGravity() const {
    return gravity;
}

trippin::Point<double> trippin::Object::getFriction() const {
    return friction;
}

trippin::Rect<int> trippin::Object::getRoundedBox() const {
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
    previousCollisions = collisions;
    collisions.clear();
}

trippin::Point<int> trippin::Object::getSize() const {
    return size;
}

void trippin::Object::setFriction(const Point<double> &f) {
    friction = f;
}

void trippin::Object::setGravity(const Point<double> &g) {
    gravity = g;
}

trippin::Sides trippin::Object::getPlatformCollisions() const {
    return platformCollisions;
}

trippin::Point<int> trippin::Object::getRoundedPosition() const {
    return roundedPosition;
}

trippin::Point<double> trippin::Object::getCenter() const {
    return center;
}

trippin::Point<int> trippin::Object::getRoundedCenter() const {
    return roundedCenter;
}

trippin::Point<double> trippin::Object::getAcceleration() const {
    return acceleration;
}

trippin::Sides trippin::Object::getObjectCollisions() const {
    return objectCollisions;
}

void trippin::Object::onPlatformCollision(trippin::Object &other, const trippin::Sides &collision) {

}

void trippin::Object::onObjectCollision(trippin::Object &other, const trippin::Sides &collision) {

}

int trippin::Object::getId() const {
    return id;
}

void trippin::Object::setPlatformCollisionType(trippin::PlatformCollisionType type) {
    platformCollisionType.set(type);
}

bool trippin::Object::collidedPreviously(const Object *obj) const {
    return std::find(previousCollisions.begin(), previousCollisions.end(), obj) != previousCollisions.end();
}
