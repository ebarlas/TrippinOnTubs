#include <thread>
#include "Clock.h"

trippin::Clock::Clock(std::chrono::milliseconds period) : tickPeriod(period), timer("engine") {
}

void trippin::Clock::next() {
    using namespace std::literals;
    auto elapsed = timer.next();
    auto targetTicks = timer.getTotalTicks() * tickPeriod;
    auto sleep = targetTicks > elapsed ? targetTicks - elapsed : 0ms;
    if (sleep > 0ms) {
        std::this_thread::sleep_for(sleep);
    }
}

int trippin::Clock::getTicks() const {
    return timer.getTotalTicks();
}