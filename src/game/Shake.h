#ifndef TRIPPIN_SHAKE_H
#define TRIPPIN_SHAKE_H

#include <vector>

// This camera shake algorithm was provided by Jonny Morril in the blog post below
// https://jonny.morrill.me/en/blog/gamedev-how-to-implement-a-camera-shake-effect/
namespace trippin {
    class Shake {
    public:
        void init(int period, int duration);
        void start(int ticks);
        void update(int ticks);
        float amplitude();
    private:
        int duration;
        int period;
        bool started;
        int startTime;
        int t;
        std::vector<float> samples;
    };
}

#endif
