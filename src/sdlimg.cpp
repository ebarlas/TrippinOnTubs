#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include "SDL.h"
#include "SDL_image.h"

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

    auto makeTexture = [renderer](const char *path) {
        auto surface = IMG_Load(path);
        if (surface == nullptr) {
            SDL_Log("Unable to load image %s. SDL_image Error: %s", path, IMG_GetError());
            std::terminate();
        }
        auto texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == nullptr) {
            SDL_Log("Unable to create texture from image. SDL Error: %s", SDL_GetError());
            std::terminate();
        }
        SDL_FreeSurface(surface);
        return texture;
    };

    std::vector<SDL_Texture *> textures;
    textures.push_back(makeTexture("sprites/zombie/zombie_3x.png"));

    auto now = [] { return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()); };
    SDL_Event e;

    SDL_Log("starting...");

    auto lastSecond = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::steady_clock::now());
    auto thisSecond = lastSecond;
    int frames = 0;
    int counter = 0;

    std::atomic_bool quit = false;

    auto thread = std::thread([&quit]() {
        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> uniform_dist(1, 6);
        int sum = 0;
        while (!quit) {
            sum += uniform_dist(e1);
        }
        SDL_Log("sum=%d", sum);
    });

    while (!quit) {
        auto t1 = now();
        auto events = SDL_PollEvent(&e);
        if (events != 0 && e.type == SDL_QUIT) {
            quit = true;
        }

//        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        auto t2 = now();
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

//        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        int rows = 10;
        int cols = 10;
        int rowHeight = 900 / rows;
        int colWidth = 1600 / cols;

        for (int c = 0; c < cols; c++) {
            for (int r = 0; r < rows; r++) {
                SDL_Rect clip{((counter + c) % 12) * 165, 0, 165, 165};
                SDL_Rect target{c * colWidth, r * rowHeight, clip.w, clip.h};
                SDL_RenderCopyEx(renderer, textures[0], &clip, &target, 0, nullptr, SDL_FLIP_NONE);
            }
        }

        auto t3 = now();
        SDL_RenderPresent(renderer);

        auto t4 = now();
        if ((t4 - t3).count() > 20'000) {
            SDL_Log("poll=%lld, render=%lld, present=%lld", (t2 - t1).count(), (t3 - t2).count(), (t4 - t3).count());
        }

        auto thisSecond = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::steady_clock::now());
        if (thisSecond == lastSecond) {
            frames++;
        } else {
            SDL_Log("fps=%d", frames);
            frames = 1;
            lastSecond = thisSecond;
        }

        counter++;
    }

    thread.join();

    for (auto texture: textures) {
        SDL_DestroyTexture(texture);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}