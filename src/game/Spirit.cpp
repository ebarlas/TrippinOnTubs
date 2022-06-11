#include "Spirit.h"

void trippin::Spirit::setVelocity(int_fast64_t vel) {
    velocity = vel;
}

void trippin::Spirit::delay(int seconds) {
    position -= velocity * ticksPerSecond * seconds;
}

int_fast64_t trippin::Spirit::getPosition() const {
    return position;
}

void trippin::Spirit::beforeTick(Uint32) {
    position += velocity;
}

void trippin::Spirit::afterTick(Uint32) {

}

int_fast64_t trippin::Spirit::getVelocity() const {
    return velocity;
}

void trippin::Spirit::setTicksPerSecond(int tps) {
    ticksPerSecond = tps;
}
