#ifndef TRIPPIN_LOCK_H
#define TRIPPIN_LOCK_H

#include "SDL.h"

namespace trippin {
    class Lock {
    public:
        Lock(SDL_mutex *mutex);
        ~Lock();
    private:
        SDL_mutex *mutex;
    };
}

#endif
