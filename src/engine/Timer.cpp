#include "Timer.h"

trippin::Timer::Timer(std::function<void(int)> oncePerSecFn) : oncePerSecFn(std::move(oncePerSecFn)) {
    totalTicks = 0;
    secondTicks = 0;
    auto now = std::chrono::steady_clock::now();
    startTime = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    lastSecond = std::chrono::time_point_cast<std::chrono::seconds>(now);
}

std::chrono::microseconds trippin::Timer::next() {
    auto now = std::chrono::steady_clock::now();
    auto thisSecond = std::chrono::time_point_cast<std::chrono::seconds>(now);
    totalTicks++;
    if (thisSecond == lastSecond) {
        secondTicks++;
    } else {
        oncePerSecFn(secondTicks);
        secondTicks = 1;
    }
    lastSecond = thisSecond;
    return std::chrono::time_point_cast<std::chrono::microseconds>(now) - startTime;
}

int trippin::Timer::getTotalTicks() const {
    return totalTicks;
}