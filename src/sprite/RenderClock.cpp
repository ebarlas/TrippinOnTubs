#include "RenderClock.h"

void trippin::RenderClock::init() {
    time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    paused = false;
}

void trippin::RenderClock::update() {
    if (!paused) {
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
        auto elapsed = now - time;
        time = now;
        ticks += elapsed;
    }
}

void trippin::RenderClock::pause() {
    paused = true;
}

void trippin::RenderClock::resume() {
    time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    paused = false;
}

bool trippin::RenderClock::isPaused() const {
    return paused;
}

std::chrono::milliseconds trippin::RenderClock::getTicks() const {
    return ticks;
}
