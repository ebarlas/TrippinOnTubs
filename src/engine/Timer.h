#ifndef TRIPPIN_TIMER_H
#define TRIPPIN_TIMER_H

#include <string>
#include "SDL.h"

namespace trippin {
    class Timer {
    public:
        Timer(const char *name);

        // Advances the timer by one tick
        // Returns the number of milliseconds elapsed since the timer started
        Uint32 next();

        Uint32 getTotalTicks() const;
    private:
        // The timer name for logging
        const char *name;

        // The total number of ticks elapsed
        Uint32 totalTicks;

        // The total number of timer seconds elapsed
        Uint32 secondTicks;

        // The SDL-millis at timer start time
        Uint32 firstMillis;

        // The previous timer second
        Uint32 lastSecond;
    };
}

#endif
