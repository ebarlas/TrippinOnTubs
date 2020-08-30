#include <exception>
#include "Lock.h"

trippin::Lock::Lock(SDL_mutex *mutex) : mutex(mutex) {
    if (SDL_LockMutex(mutex)) {
        SDL_Log("Couldn't lock mutex");
        std::terminate();
    }
}

trippin::Lock::~Lock() {
    SDL_UnlockMutex(mutex);
}
