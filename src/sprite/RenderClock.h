#ifndef TRIPPIN_RENDERCLOCK_H
#define TRIPPIN_RENDERCLOCK_H

#include "SDL.h"

namespace trippin {
    class RenderClock {
    public:
        void init();
        void update();
        void pause();
        void resume();
        int getElapsed() const;
        Uint32 getTicks() const;
        bool isPaused() const;
    private:
        Uint32 time;
        Uint32 elapsed;
        Uint32 ticks;
        bool paused;
    };
}

#endif
