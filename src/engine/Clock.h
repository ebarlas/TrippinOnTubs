#ifndef TRIPPIN_CLOCK_H
#define TRIPPIN_CLOCK_H

#include "SDL.h"

namespace trippin {
    // Engine clock for tracking various time components from the engine perspective
    class Clock {
    private:
        // The target tick period in milliseconds
        // An engine tick ought to occur at this frequency
        // With a target tick rate of 100 per second, the tick period would be 10
        Uint32 tickPeriod;

        // The total number of engine ticks elapsed
        Uint32 totalTicks;

        // The total number of engine seconds elapsed
        Uint32 secondTicks;

        // The SDL-millis at engine thread start time
        Uint32 firstMillis;

        // The previous engine second
        Uint32 lastSecond;
    public:
        Clock(Uint32 tickPeriod);
        void next();
        Uint32 getTicks() const;
    };
}

#endif
