#ifndef TRIPPIN_RENDERCLOCK_H
#define TRIPPIN_RENDERCLOCK_H

#include <chrono>

namespace trippin {
    class RenderClock {
    public:
        void init();
        void update();
        void pause();
        void resume();
        int getTicks() const;
        bool isPaused() const;
    private:
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> time;
        std::chrono::milliseconds elapsed;
        std::chrono::milliseconds ticks;
        bool paused;
    };
}

#endif
