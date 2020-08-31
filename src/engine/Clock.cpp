#include "Clock.h"

trippin::Clock::Clock() {
    totalTicks = 0;
    secondTicks = 0;
    firstMillis = SDL_GetTicks();
    lastSecond = firstMillis / 1000;
}

void trippin::Clock::next() {
    auto nowMillis = SDL_GetTicks();
    auto thisSecond = nowMillis / 1000;
    totalTicks++;
    if (thisSecond == lastSecond) {
        secondTicks++;
    } else {
        SDL_Log("now=%d, tps=%d", nowMillis, secondTicks);
        secondTicks = 1;
    }
    lastSecond = thisSecond;
    auto elapsed = nowMillis - firstMillis;
    auto targetTicks = totalTicks * tickPeriod;
    auto sleep = targetTicks > elapsed ? targetTicks - elapsed : 0;
    if (sleep > 0) {
        SDL_Delay(sleep);
    }
}

Uint32 trippin::Clock::getTicks() const {
    return totalTicks;
}

void trippin::Clock::setTickPeriod(Uint32 period) {
    tickPeriod = period;
}

Uint32 trippin::Clock::getTickPeriod() const {
    return tickPeriod;
}
