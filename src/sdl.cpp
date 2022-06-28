#include <chrono>
#include "SDL.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize. SDL error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
            "Test",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1600,
            900,
            SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Log("Window could not be created. SDL error: %s", SDL_GetError());
        return 2;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created. SDL error: %s", SDL_GetError());
        return 3;
    }

    auto now = [] { return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()); };
    SDL_Event e;
    bool quit = false;

    SDL_Log("starting...");

    while (!quit) {
        auto t1 = now();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        auto t2 = now();
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        auto t3 = now();
        SDL_RenderPresent(renderer);

        auto t4 = now();
        if ((t4 - t3).count() > 20'000) {
            SDL_Log("poll=%lld, render=%lld, present=%lld", (t2 - t1).count(), (t3 - t2).count(), (t4 - t3).count());
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}