#include <thread>
#include "SDL.h"
#include "Clock.h"

static void logTps(int tps) {
    SDL_Log("timer=engine, tps=%d", tps);
}

trippin::Clock::Clock(int tickRate) :
        tickRate(tickRate),
        tickPeriod(tickRateToTickPeriod(tickRate)),
        timer(logTps) {
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
    return priorTicks + timer.getTotalTicks();
}

void trippin::Clock::updateTickRate(int tr) {
    tickRate = tr;
    tickPeriod = tickRateToTickPeriod(tr);
    priorTicks += timer.getTotalTicks();
    timer = Timer{logTps};
}

std::chrono::microseconds trippin::Clock::tickRateToTickPeriod(int tickRate) {
    return std::chrono::microseconds{1'000'000 / tickRate};
}

int trippin::Clock::getTickRate() const {
    return tickRate;
}
