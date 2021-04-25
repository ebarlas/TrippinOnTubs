#ifndef TRIPPIN_INTERPOLATOR_H
#define TRIPPIN_INTERPOLATOR_H

#include "SDL.h"

namespace trippin {
    class Interpolator {
    public:
        Interpolator(Uint32 duration, int magnitude);
        void reset();
        int interpolate() const;
    private:
        int magnitude;
        Uint32 duration;
        Uint32 firstTick;
        static float interpolate(float input);
    };
}

#endif
