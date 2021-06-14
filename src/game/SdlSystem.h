#ifndef TRIPPIN_SDLSYSTEM_H
#define TRIPPIN_SDLSYSTEM_H

#include "SDL.h"

namespace trippin {
    class SdlSystem {
    public:
        SdlSystem();
        ~SdlSystem();
        SDL_Window *getWindow() {
            return window;
        }
        SDL_Renderer *getRenderer() {
            return renderer;
        }
        SDL_Point getWindowSize() {
            return windowSize;
        }
        SDL_Point getRendererSize() {
            return rendererSize;
        }
    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Point windowSize;
        SDL_Point rendererSize;
    };
}

#endif
