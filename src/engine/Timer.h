#ifndef TRIPPIN_TIMER_H
#define TRIPPIN_TIMER_H

#include <chrono>
#include <functional>

namespace trippin {
    class Timer {
    public:
        Timer();

        // Advances the timer by one tick
        // Returns the number of ticks elapsed since the timer started
        int next(const std::function<void(int)> &fn);
    private:
        // The total number of ticks elapsed
        int totalTicks;

        // The number of ticks elapsed this second, passed to function on second transitions
        int secondTicks;

        // The previous timer second, used for calculating ticks per second for logging
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::seconds> lastSecond;
    };
}

#endif
