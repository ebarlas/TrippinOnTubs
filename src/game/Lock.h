#ifndef TRIPPIN_LOCK_H
#define TRIPPIN_LOCK_H

#include "Mutex.h"

namespace trippin {
    class Lock {
    public:
        Lock(Mutex &mutex);
        ~Lock();
    private:
        SDL_mutex *mutex;
    };
}

#endif
