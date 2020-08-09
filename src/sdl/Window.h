#ifndef TRIPPIN_WINDOW_H
#define TRIPPIN_WINDOW_H

#include "SDL.h"

namespace trippin {
    struct Window {
        SDL_Window *window;
        Window(const char *title, int x, int y, int w, int h, Uint32 flags);
        ~Window();
    };
}

#endif
