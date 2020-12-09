#include "Spirit.h"

void trippin::Spirit::setPosition(double pos) {
    position = pos;
}

void trippin::Spirit::setVelocity(double vel) {
    velocity = vel;
}

void trippin::Spirit::updatePosition(double delta) {
    position += delta;
}

double trippin::Spirit::getPosition() const {
    return position;
}

void trippin::Spirit::beforeTick(Uint32 engineTicks) {
    position += velocity;
}

void trippin::Spirit::afterTick(Uint32 engineTicks) {

}

bool trippin::Spirit::isExpired() {
    return false;
}
