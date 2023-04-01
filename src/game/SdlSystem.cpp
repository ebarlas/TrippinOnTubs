#include <exception>
#include <tuple>
#include <string>
#include "SDL_mixer.h"
#include "SdlSystem.h"

static std::tuple<int, int, unsigned int> getWinParams(const std::string &platform, SDL_Point ws) {
    if (platform == "iOS") {
        return {0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI};
    } else if (platform == "Android") {
        return {0, 0, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN};
    } else if (platform == "Mac OS X") {
        return {ws.x, ws.y, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI};
    } else {
        return {ws.x, ws.y, SDL_WINDOW_SHOWN};
    }
}

static std::tuple<int, int> getWinSize(const std::string &platform, SDL_Renderer* renderer) {
    if (platform == "Android") {
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);
        return {displayMode.w, displayMode.h};
    } else {
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);
        return {w, h};
    }
}

static int getDpiScale(const std::string &platform, SDL_Window* window, int winWidth) {
    if (platform == "Mac OS X") {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return winWidth / w;
    } else {
        return 1;
    }
}

trippin::SdlSystem::SdlSystem(SDL_Point ws) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL could not initialize. SDL error: %s", SDL_GetError());
        std::terminate();
    }

    auto platform = std::string(SDL_GetPlatform());
    auto [winx, winy, winflags] = getWinParams(platform, ws);

    window = SDL_CreateWindow(
            "Trippin on Tubs",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            winx,
            winy,
            winflags);
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

    auto [winw, winh] = getWinSize(platform, renderer);
    windowSize = {winw, winh};
    highDpiScale = getDpiScale(platform, window, winw);

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
