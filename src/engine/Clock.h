#ifndef TRIPPIN_CLOCK_H
#define TRIPPIN_CLOCK_H

#include "SDL.h"
#include "Timer.h"

namespace trippin {
    // Engine clock for tracking various time components from the engine perspective
    class Clock {
    private:
        // The target tick period in milliseconds
        // An engine tick ought to occur at this frequency
        // With a target tick rate of 100 per second, the tick period would be 10
        const std::chrono::milliseconds tickPeriod;

        Timer timer;
    public:
        Clock(std::chrono::milliseconds tickPeriod);
        void next();
        int getTicks() const;
    };
}

#endif
