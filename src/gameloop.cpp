#include "engine/Sides.h"
#include "gameloop.h"

void gameLoop(
        SDL_Renderer *ren,
        const trippin::Vector<int> &windowSize,
        const std::function<void(const GameState &)> &initFn,
        const std::function<void(const GameState &)> &updateFn,
        const std::function<void(const GameState &)> &renderFn) {
    trippin::FontRenderer fontRenderer{ren};

    initFn({ren, &fontRenderer, windowSize});

    trippin::Sides keysDown;
    bool debug = false;
    bool quit = false;
    while (!quit) {
        SDL_Event e;
        bool enterPressed = false;
        bool spacePressed = false;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    enterPressed = true;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    spacePressed = true;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                    keysDown.setLeft(true);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
                    keysDown.setTop(true);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                    keysDown.setRight(true);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    keysDown.setBottom(true);
                }
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                    keysDown.setLeft(false);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
                    keysDown.setTop(false);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                    keysDown.setRight(false);
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    keysDown.setBottom(false);
                }
            }
        }

        if (enterPressed) {
            debug = !debug;
        }

        if (!debug || spacePressed) {
            updateFn({ren, &fontRenderer, windowSize, keysDown});
        }

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        renderFn({ren, &fontRenderer, windowSize, keysDown});

        SDL_RenderPresent(ren);
    }
}

void gameLoop(const std::function<void(const GameState &)> &initFn,
             const std::function<void(const GameState &)> &updateFn,
             const std::function<void(const GameState &)> &renderFn) {
    trippin::Vector<int> windowSize{1600, 900};
    trippin::Vector<int> windowPos{100, 100};
    auto title = "Hacking";

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow(title, windowPos.x, windowPos.y, windowSize.x, windowSize.y, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Init();

    gameLoop(ren, windowSize, initFn, updateFn, renderFn);

    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}