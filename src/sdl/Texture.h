#ifndef TRIPPIN_TEXTURE_H
#define TRIPPIN_TEXTURE_H

#include "SDL.h"

namespace trippin {
    struct Texture {
        SDL_Texture *texture;
        Texture(SDL_Renderer *renderer, SDL_Surface *surface);
        ~Texture();
    };
}

#endif
