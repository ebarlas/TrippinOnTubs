#ifndef TRIPPIN_TIMER_H
#define TRIPPIN_TIMER_H

#include <chrono>

namespace trippin {
    class Timer {
    public:
        Timer(const char *name);

        // Advances the timer by one tick
        // Returns the number of milliseconds elapsed since the timer started
        std::chrono::milliseconds next();

        int getTotalTicks() const;
    private:
        // The timer name for logging
        const char *name;

        // The total number of ticks elapsed
        int totalTicks;

        // The number of ticks elapsed this second, used for logging only
        int secondTicks;

        // The clock millis at timer start time
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> firstMillis;

        // The previous timer second, used for calculating ticks per second for logging
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::seconds> lastSecond;
    };
}

#endif
