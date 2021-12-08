#ifndef TRIPPIN_INTERPOLATOR_H
#define TRIPPIN_INTERPOLATOR_H

#include "sprite/RenderClock.h"

namespace trippin {
    class Interpolator {
    public:
        Interpolator(const RenderClock& renderClock, int duration, int magnitude = 0);
        void setMagnitude(int magnitude);
        void reset();
        int interpolate() const;
    private:
        const RenderClock& renderClock;
        int magnitude;
        int duration;
        int firstTick;
        static float interpolate(float input);
    };
}

#endif
