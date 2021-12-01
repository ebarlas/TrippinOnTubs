#ifndef TRIPPIN_SDLSYSTEM_H
#define TRIPPIN_SDLSYSTEM_H

#include "SDL.h"

namespace trippin {
    // Provides simple lifecycle management for SDL entities
    // Only one instance of this type ought to exist in the program
    class SdlSystem {
    public:
        SdlSystem();
        ~SdlSystem();
        SdlSystem(const SdlSystem&) = delete;
        SdlSystem(const SdlSystem&&) = delete;
        SdlSystem& operator=(const SdlSystem&) = delete;
        SdlSystem& operator=(const SdlSystem&&) = delete;
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
