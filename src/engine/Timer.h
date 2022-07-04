#ifndef TRIPPIN_TIMER_H
#define TRIPPIN_TIMER_H

#include <chrono>
#include <functional>

namespace trippin {
    class Timer {
    public:
        Timer(std::function<void(int)> oncePerSecFn);

        // Advances the timer by one tick
        // Returns the number of microseconds elapsed since the timer started
        std::chrono::microseconds next();

        int getTotalTicks() const;
    private:
        // Function to run on each second transition
        // Useful for logging diagnostics at a fixed schedule
        std::function<void(int)> oncePerSecFn;

        // The total number of ticks elapsed
        int totalTicks;

        // The number of ticks elapsed this second, passed to function on second transitions
        int secondTicks;

        // The clock microseconds at timer start time
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds> startTime;

        // The previous timer second, used for calculating ticks per second for logging
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::seconds> lastSecond;
    };
}

#endif
