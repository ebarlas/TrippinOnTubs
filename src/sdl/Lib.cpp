#include <exception>
#include "SDL.h"
#include "Lib.h"

trippin::Lib::Lib(Uint32 flags) {
    if (SDL_Init(flags) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s", SDL_GetError());
        std::terminate();
    }
}

trippin::Lib::~Lib() {
    SDL_Quit();
}
