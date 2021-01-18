#include "Timer.h"

trippin::Timer::Timer(const char *timerName) {
    name = timerName;
    totalTicks = 0;
    secondTicks = 0;
    firstMillis = SDL_GetTicks();
    lastSecond = firstMillis / 1000;
}

Uint32 trippin::Timer::next() {
    auto nowMillis = SDL_GetTicks();
    auto thisSecond = nowMillis / 1000;
    totalTicks++;
    if (thisSecond == lastSecond) {
        secondTicks++;
    } else {
        SDL_Log("timer=%s, now=%d, tps=%d", name, nowMillis, secondTicks);
        secondTicks = 1;
    }
    lastSecond = thisSecond;
    return nowMillis - firstMillis;
}

Uint32 trippin::Timer::getTotalTicks() const {
    return totalTicks;
}
