#ifndef TRIPPIN_GAMELOOP_H
#define TRIPPIN_GAMELOOP_H

#include "SDL.h"
#include "FontRenderer.h"

struct GameState {
    SDL_Renderer *renderer{};
    trippin::FontRenderer *fontRenderer{};
    trippin::Point<int> windowSize{};
    trippin::Sides keysDown{};
    Uint32 ticks{};
};

void gameLoop(
        SDL_Renderer *ren,
        const trippin::Point<int> &windowSize,
        const std::function<void(const GameState &)> &initFn,
        const std::function<void(const GameState &)> &updateFn,
        const std::function<void(const GameState &)> &renderFn);


void gameLoop(
        const std::function<void(const GameState &)> &initFn,
        const std::function<void(const GameState &)> &updateFn,
        const std::function<void(const GameState &)> &renderFn,
        bool fullScreen = false);

#endif