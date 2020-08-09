#include <exception>
#include "Window.h"

trippin::Window::Window(const char *title, int x, int y, int w, int h, Uint32 flags) {
    window = SDL_CreateWindow(title, x, y, w, h, flags);
    if (window == nullptr) {
        SDL_Log("Window could not be created! SDL Error: %s", SDL_GetError());
        std::terminate();
    }
}

trippin::Window::~Window() {
    SDL_DestroyWindow(window);
}
