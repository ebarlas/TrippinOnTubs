#ifndef TRIPPIN_SHAKE_H
#define TRIPPIN_SHAKE_H

#include <vector>
#include "SDL.h"

// This camera shake algorithm was provided by Jonny Morril in the blog post below
// https://jonny.morrill.me/en/blog/gamedev-how-to-implement-a-camera-shake-effect/
namespace trippin {
    class Shake {
    public:
        void init(int period, int duration);
        void start(Uint32 ticks);
        void update(Uint32 ticks);
        float amplitude();
        float decay();
    private:
        int duration;
        int period;
        bool started;
        Uint32 startTime;
        Uint32 t;
        std::vector<float> samples;
    };
}

#endif
