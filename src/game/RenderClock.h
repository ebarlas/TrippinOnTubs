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
        int getElapsed();
    private:
        Uint32 time;
        Uint32 pauseTime;
        Uint32 elapsed;
        bool paused;
    };
}

#endif
