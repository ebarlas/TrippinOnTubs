#ifndef TRIPPIN_INTERPOLATOR_H
#define TRIPPIN_INTERPOLATOR_H

#include "sprite/RenderClock.h"

namespace trippin {
    class Interpolator {
    public:
        Interpolator(const RenderClock& renderClock, int duration, int magnitude = 0, int offset = 0);
        void setMagnitude(int magnitude);
        void setOffset(int offset);
        void reset();
        int interpolate() const;
    private:
        const RenderClock& renderClock;
        const int duration;
        int offset;
        int magnitude;
        std::chrono::milliseconds firstTick;
        static float interpolate(float input);
    };
}

#endif
