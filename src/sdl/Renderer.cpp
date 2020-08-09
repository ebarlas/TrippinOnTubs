#include <exception>
#include "Renderer.h"

trippin::Renderer::Renderer(SDL_Window *window, int index, Uint32 flags) {
    renderer = SDL_CreateRenderer(window, index, flags);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created! SDL Error: %s", SDL_GetError());
        std::terminate();
    }
}

trippin::Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}
