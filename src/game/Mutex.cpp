#include <exception>
#include "Mutex.h"

trippin::Mutex::Mutex() {
    mutex = SDL_CreateMutex();
    if (!mutex) {
        SDL_Log("Couldn't create mutex");
        std::terminate();
    }
}

trippin::Mutex::~Mutex() {
    SDL_DestroyMutex(mutex);
}

SDL_mutex *trippin::Mutex::get() {
    return mutex;
}
