#include <iostream>
#include <vector>
#include <cmath>
#include "engine/Engine.h"
#include "engine/Collisions.h"

std::ostream &operator<<(std::ostream &os, const trippin::Object &obj) {
    return os << "obj[id=" << obj.id << ", pos=(" << obj.position << "), vel=(" << obj.velocity << ")]";
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize. SDL Error: %s", SDL_GetError());
        std::terminate();
    }

    trippin::Point<int> windowSize = {1600, 900};

    SDL_Window *window = SDL_CreateWindow(
            "Trippin on Tubs",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            windowSize.x,
            windowSize.y,
            SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Log("Window could not be created. SDL Error: %s", SDL_GetError());
        std::terminate();
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created. SDL Error: %s", SDL_GetError());
        std::terminate();
    }

    trippin::Object objects[2];
    objects[0].id = 1;
    objects[0].platform = false;
    objects[0].mass = 10;
    objects[0].position = {15, 10};
    objects[0].velocity = {0.2, 0.2};
    objects[0].terminalVelocity = {1000, 1000}; // arbitrary high ceiling
    objects[0].size = {10, 10};

    objects[1].id = 2;
    objects[1].platform = false;
    objects[1].mass = 10;
    objects[1].position = {30, 30};
    objects[1].velocity = {-0.2, -0.2};
    objects[1].terminalVelocity = {1000, 1000}; // arbitrary high ceiling
    objects[1].size = {10, 10};

    std::cout << objects[0] << ", " << objects[1] << std::endl;

    trippin::Engine engine;
    engine.setPlatformCollision(trippin::onReflectiveCollisionDefault);
    engine.setObjectCollision(trippin::onElasticCollision2D);
    engine.add(&objects[0]);
    engine.add(&objects[1]);

    Uint32 tick = 1;

    int velScale = 600;
    int objScale = 20;
    int minorTicks = 20;
    int majorTicks = 20;

    /*
    // Uncomment to run with formerly oscillating collisions
    objects[0].mass = 9030;
    objects[0].size = {129, 70};
    objects[0].position = {161.40, 79.87};
    objects[0].velocity = {1.79, -1.05};
    objects[1].mass = 8280;
    objects[1].size = {138, 60};
    objects[1].position = {61.05, 20.11};
    objects[1].velocity = {-0.33, 1.00};
    objScale = 5;
    */

    objects[0].syncPositions();
    objects[1].syncPositions();

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    engine.tick(tick);
                    std::cout << objects[0] << ", " << objects[1] << std::endl;
                    tick++;
                }
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {

                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
        for (int x = 0; x < windowSize.x; x += minorTicks) {
            SDL_RenderDrawLine(renderer, x, 0, x, windowSize.y);
        }
        for (int y = 0; y < windowSize.y; y += minorTicks) {
            SDL_RenderDrawLine(renderer, 0, y, windowSize.x, y);
        }

        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        for (int x = 0; x < windowSize.x; x += majorTicks) {
            SDL_RenderDrawLine(renderer, x, 0, x, windowSize.y);
        }
        for (int y = 0; y < windowSize.y; y += majorTicks) {
            SDL_RenderDrawLine(renderer, 0, y, windowSize.x, y);
        }

        for (auto &obj : objects) {
            auto vel = obj.velocity * static_cast<double>(velScale);
            auto center = obj.center * static_cast<double>(objScale);
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderDrawLine(
                    renderer,
                    static_cast<int>(std::round(center.x)),
                    static_cast<int>(std::round(center.y)),
                    static_cast<int>(std::round(center.x + vel.x)),
                    static_cast<int>(std::round(center.y + vel.y)));

            auto rounded = obj.roundedBox * objScale;
            SDL_Rect sr{rounded.x, rounded.y, rounded.w, rounded.h};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &sr);

            auto p = obj.position * static_cast<double>(objScale);
            sr = {static_cast<int>(std::round(p.x)), static_cast<int>(std::round(p.y)), rounded.w, rounded.h};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &sr);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderDrawLine(
                renderer,
                static_cast<int>(std::round(objects[0].center.x * objScale)),
                static_cast<int>(std::round(objects[0].center.y * objScale)),
                static_cast<int>(std::round(objects[1].center.x * objScale)),
                static_cast<int>(std::round(objects[1].center.y * objScale)));

        SDL_RenderPresent(renderer);
    }
}