#include "RenderClock.h"

void trippin::RenderClock::init() {
    elapsed = 0;
    time = SDL_GetTicks();
    paused = false;
}

void trippin::RenderClock::update() {
    if (!paused) {
        auto now = SDL_GetTicks();
        elapsed = now - time;
        time = now;
    } else {
        elapsed = 0;
    }
}

int trippin::RenderClock::getElapsed() {
    return elapsed;
}

void trippin::RenderClock::pause() {
    paused = true;
}

void trippin::RenderClock::resume() {
    time = SDL_GetTicks();
    paused = false;
}
