#ifndef TRIPPIN_CLOCK_H
#define TRIPPIN_CLOCK_H

#include "Timer.h"

namespace trippin {
    // Engine clock throttles the pace of the engine thread to match a target tick rate via the next() function
    class Clock {
    private:
        // The target tick rate, such as 100 ticks per second
        int tickRate;

        // The target tick period in microseconds
        // An engine tick ought to occur at this frequency
        // With a target tick rate of 100 per second, the tick period would be 10 milliseconds or 10,000 microseconds
        std::chrono::microseconds tickPeriod;

        // Timer used for calculating ticks and time elapsed
        Timer timer;

        // Prior ticks accumulated during tick-rate changes
        int priorTicks{};

        static std::chrono::microseconds tickRateToTickPeriod(int tickRate);
    public:
        Clock(int tickRate);
        void next();
        int getTicks() const;
        int getTickRate() const;
        void updateTickRate(int tickRate);
    };
}

#endif
