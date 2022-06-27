#include <thread>
#include "SDL.h"
#include "Clock.h"

trippin::Clock::Clock(int tickRate) :
        tickPeriod(std::chrono::microseconds{1'000'000 / tickRate}),
        timer([](int tps) { SDL_Log("timer=engine, tps=%d", tps); }) {
}

void trippin::Clock::next() {
    auto zero = std::chrono::microseconds{0};
    auto elapsed = timer.next();
    auto targetTicks = timer.getTotalTicks() * tickPeriod;
    auto sleep = targetTicks > elapsed ? targetTicks - elapsed : zero;
    if (sleep > zero) {
        std::this_thread::sleep_for(sleep);
    }
}

int trippin::Clock::getTicks() const {
    return timer.getTotalTicks();
}