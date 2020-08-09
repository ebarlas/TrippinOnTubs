#ifndef TRIPPIN_RENDERER_H
#define TRIPPIN_RENDERER_H

#include "SDL.h"

namespace trippin {
    struct Renderer {
        SDL_Renderer *renderer;
        Renderer(SDL_Window *window, int index, Uint32 flags);
        ~Renderer();
    };
}

#endif
