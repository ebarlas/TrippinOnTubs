#include "SDL.h"
#include "SDL2/SDL_ttf.h"
#include "engine/object.h"

void gameLoop(SDL_Renderer *ren, const trippin::Vector<int> &windowSize) {
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        // render here!

        SDL_RenderPresent(ren);
    }
}

int main() {
    trippin::Vector<int> windowSize{1600, 900};
    trippin::Vector<int> windowPos{100, 100};
    auto title = "Hacking";

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow(title, windowPos.x, windowPos.y, windowSize.x, windowSize.y, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Init();

    gameLoop(ren, windowSize);

    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}