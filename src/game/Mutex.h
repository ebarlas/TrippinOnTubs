#ifndef TRIPPIN_MUTEX_H
#define TRIPPIN_MUTEX_H

#include "SDL.h"

namespace trippin {
    class Mutex {
    private:
        SDL_mutex *mutex;
    public:
        Mutex();
        ~Mutex();
        SDL_mutex *get() const;
    };
}

#endif
