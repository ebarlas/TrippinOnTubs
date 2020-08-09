#ifndef TRIPPIN_SURFACE_H
#define TRIPPIN_SURFACE_H

#include "SDL.h"

namespace trippin {
    struct Surface {
        SDL_Surface *surface;
        Surface(const char *file);
        ~Surface();
    };
}

#endif
