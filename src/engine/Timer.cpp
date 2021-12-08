#include "Timer.h"
#include "SDL.h"

trippin::Timer::Timer(const char *timerName) {
    name = timerName;
    totalTicks = 0;
    secondTicks = 0;
    auto now = std::chrono::steady_clock::now();
    firstMillis = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    lastSecond = std::chrono::time_point_cast<std::chrono::seconds>(now);
}

std::chrono::milliseconds trippin::Timer::next() {
    auto now = std::chrono::steady_clock::now();
    auto thisSecond = std::chrono::time_point_cast<std::chrono::seconds>(now);
    totalTicks++;
    if (thisSecond == lastSecond) {
        secondTicks++;
    } else {
        SDL_Log("timer=%s, tps=%d", name, secondTicks);
        secondTicks = 1;
    }
    lastSecond = thisSecond;
    return std::chrono::time_point_cast<std::chrono::milliseconds>(now) - firstMillis;
}

int trippin::Timer::getTotalTicks() const {
    return totalTicks;
}
