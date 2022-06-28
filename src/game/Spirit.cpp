#include "Spirit.h"

void trippin::Spirit::setVelocity(double vel) {
    velocity = vel;
}

void trippin::Spirit::delay(double seconds) {
    position -= velocity * ticksPerSecond * seconds;
}

double trippin::Spirit::getPosition() const {
    return position;
}

void trippin::Spirit::beforeTick(int) {
    position += velocity;
}

void trippin::Spirit::afterTick(int) {

}

bool trippin::Spirit::isExpired() {
    return false;
}

double trippin::Spirit::getVelocity() const {
    return velocity;
}

void trippin::Spirit::setTicksPerSecond(double tps) {
    ticksPerSecond = tps;
}
