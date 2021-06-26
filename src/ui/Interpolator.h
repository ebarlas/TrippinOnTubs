#ifndef TRIPPIN_INTERPOLATOR_H
#define TRIPPIN_INTERPOLATOR_H

#include "sprite/RenderClock.h"

namespace trippin {
    class Interpolator {
    public:
        Interpolator(const RenderClock& renderClock, Uint32 duration, int magnitude = 0);
        void setMagnitude(int magnitude);
        void reset();
        int interpolate() const;
    private:
        const RenderClock& renderClock;
        int magnitude;
        Uint32 duration;
        Uint32 firstTick;
        static float interpolate(float input);
    };
}

#endif
