#include "Timer.h"

trippin::Timer::Timer() {
    totalTicks = 0;
    secondTicks = 0;
    lastSecond = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::steady_clock::now());
}

int trippin::Timer::next(const std::function<void(int)> &fn) {
    auto thisSecond = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::steady_clock::now());
    if (thisSecond == lastSecond) {
        secondTicks++;
    } else {
        fn(secondTicks);
        secondTicks = 1;
    }
    lastSecond = thisSecond;
    return ++totalTicks;
}