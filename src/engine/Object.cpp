#include "engine/Sides.h"
#include "engine/Object.h"
#include "engine/Convert.h"

void trippin::Object::syncPositions() {
    center = {position.x + size.x / 2.0, position.y + size.y / 2.0};
    roundedPosition = toInt(position);
    roundedBox = {roundedPosition.x, roundedPosition.y, size.x, size.y};
}

void trippin::Object::applyMotion() {
    auto &pc = platformCollisions;
    auto &oc = objectCollisions;
    if (pc.testBottom() || pc.testTop() || oc.testBottom() || oc.testTop()) {
        velocity.x = velocity.x > 0
                     ? std::max(velocity.x - friction.x, 0.0)
                     : std::min(velocity.x + friction.x, 0.0);
    }
    if (pc.testLeft() || pc.testRight() || oc.testLeft() || oc.testRight()) {
        velocity.y = velocity.y > 0
                     ? std::max(velocity.y - friction.y, 0.0)
                     : std::min(velocity.y + friction.y, 0.0);
    }

    if (fallGravity > 0 && velocity.y > 0) {
        if ((!pc.testTop() && fallGravity < 0) || (!pc.testBottom() && fallGravity > 0)) {
            velocity.y = velocity.y + fallGravity;
        }
    } else {
        if ((!pc.testTop() && gravity < 0) || (!pc.testBottom() && gravity > 0)) {
            velocity.y = velocity.y + gravity;
        }
    }

    if ((!pc.testLeft() && acceleration.x < 0) || (!pc.testRight() && acceleration.x > 0)) {
        velocity.x = velocity.x + acceleration.x;
    }
    if ((!pc.testTop() && acceleration.y < 0) || (!pc.testBottom() && acceleration.y > 0)) {
        velocity.y = velocity.y + acceleration.y;
    }

    if (velocity.y < -terminalVelocity.y) {
        velocity.y = -terminalVelocity.y;
    } else if (velocity.y > terminalVelocity.y) {
        velocity.y = terminalVelocity.y;
    }

    if (velocity.x < -terminalVelocity.x) {
        velocity.x = -terminalVelocity.x;
    } else if (velocity.x > terminalVelocity.x) {
        velocity.x = terminalVelocity.x;
    }

    position += velocity;
    syncPositions();

    platformCollisions = {};
    objectCollisions = {};
    snapCollisions.clear();
    snappedToMe = false;
}

void trippin::Object::onPlatformCollision(trippin::Object &other, const trippin::Sides &collision) {

}

void trippin::Object::onObjectCollision(trippin::Object &other, const trippin::Sides &collision) {

}

bool trippin::Object::isExpired() {
    return expired;
}
