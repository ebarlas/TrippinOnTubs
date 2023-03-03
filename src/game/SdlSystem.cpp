#include <exception>
#include "SDL_mixer.h"
#include "SdlSystem.h"

trippin::SdlSystem::SdlSystem(SDL_Point ws) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL could not initialize. SDL error: %s", SDL_GetError());
        std::terminate();
    }

    if (ws.x || ws.y) {
        windowSize = ws;
    } else {
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);
        windowSize = {displayMode.w, displayMode.h};
    }
    SDL_Log("window size w=%d, h=%d", windowSize.x, windowSize.y);

    window = SDL_CreateWindow(
            "Trippin on Tubs",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            ws.x,
            ws.y,
            ws.x || ws.y ? SDL_WINDOW_SHOWN : SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (window == nullptr) {
        SDL_Log("Window could not be created. SDL error: %s", SDL_GetError());
        std::terminate();
    }

    renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created. SDL error: %s", SDL_GetError());
        std::terminate();
    }

    SDL_DisplayMode displayMode;
    SDL_GetDisplayMode(0, 0, &displayMode);
    refreshRate = displayMode.refresh_rate;

    auto flags = MIX_INIT_MP3;
    if (Mix_Init(flags) != flags) {
        SDL_Log("Mixer could not be initialized. Mixer error: %s", Mix_GetError());
        std::terminate();
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        SDL_Log("Mixer open-audio failed. Mixer error: %s", Mix_GetError());
        std::terminate();
    }
}

trippin::SdlSystem::~SdlSystem() {
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
