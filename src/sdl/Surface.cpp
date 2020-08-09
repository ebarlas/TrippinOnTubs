#include <exception>
#include "SDL_Image.h"
#include "Surface.h"

trippin::Surface::Surface(const char *file) {
    surface = IMG_Load(file);
    if (surface == nullptr) {
        SDL_Log( "Unable to load image %s! SDL_image Error: %s", file, IMG_GetError() );
        std::terminate();
    }
}

trippin::Surface::~Surface() {
    SDL_FreeSurface(surface);
}
