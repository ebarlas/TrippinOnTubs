#include "Clock.h"

trippin::Clock::Clock(Uint32 period) : timer("engine") {
    tickPeriod = period;
}

void trippin::Clock::next() {
    auto elapsed = timer.next();
    auto targetTicks = timer.getTotalTicks() * tickPeriod;
    auto sleep = targetTicks > elapsed ? targetTicks - elapsed : 0;
    if (sleep > 0) {
        SDL_Delay(sleep);
    }
}

Uint32 trippin::Clock::getTicks() const {
    return timer.getTotalTicks();
}