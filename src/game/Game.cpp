#include "Game.h"
#include "Ground.h"
#include "PacingObject.h"
#include "WingedTub.h"
#include "Layer.h"

void trippin::Game::init() {
    initRuntime();
    initWindowSize();
    initWindow();
    initRenderer();
    initConfiguration();
    initScale();
    initSpriteManager();
    initCamera();
    initEngine();
}

void trippin::Game::initRuntime() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize. SDL Error: %s", SDL_GetError());
        std::terminate();
    }
}

void trippin::Game::initWindowSize() {
    /*
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowSize = {displayMode.w, displayMode.h};
    */
    windowSize = {1600, 900};
}

void trippin::Game::initWindow() {
    window = SDL_CreateWindow(
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
}

void trippin::Game::initRenderer() {
    renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created. SDL Error: %s", SDL_GetError());
        std::terminate();
    }
}

void trippin::Game::initConfiguration() {
    configuration.load(configName);
    map.load(configuration.map);
}

void trippin::Game::initScale() {
    auto &cs = configuration.scales;
    scale = &cs[0];
    for (int i = cs.size() - 1; i > 0; i--) {
        if (windowSize.x >= configuration.minimumViewportWidth * cs[i].getMultiplier()) {
            scale = &cs[i];
            break;
        }
    }
}

void trippin::Game::initSpriteManager() {
    spriteManager = std::make_unique<SpriteManager>(renderer, *scale);
}

void trippin::Game::initCamera() {
    auto uni = scale->scale(map.universe);
    camera.setViewport({0, 0, windowSize.x, windowSize.y});
    camera.setUniverse({0, 0, uni.x, uni.y});
}

void trippin::Game::initEngine() {
    engine.setTickPeriod(configuration.tickPeriod);
    engine.setPlatformCollision(&platformCollision);
    engine.setObjectCollision(&objectCollision);
    engine.addListener(&spirit);

    for (auto &layer : map.layers) {
        auto uptr = std::make_unique<Layer>();
        uptr->init(*spriteManager, layer);
        objects.push_back(std::move(uptr));
    }

    for (auto &obj : map.objects) {
        if (obj.type == "goggin") {
            goggin.setDust(spriteManager->get("dust"));
            goggin.init(configuration, obj, spriteManager->get(obj.type));
            engine.add(&goggin);
        } else if (obj.type.rfind("ground_melt_", 0) == 0 || obj.type.rfind("platform", 0) == 0) {
            auto uptr = std::make_unique<Ground>();
            uptr->init(configuration, obj, spriteManager->get(obj.type));
            uptr->setSpirit(&spirit);
            uptr->setActivation(&activation);
            engine.add(uptr.get());
            objects.push_back(std::move(uptr));
        } else if (obj.type == "winged_tub") {
            auto uptr = std::make_unique<WingedTub>();
            uptr->init(configuration, obj, spriteManager->get(obj.type));
            uptr->setGoggin(&goggin);
            uptr->setScore(&score);
            uptr->setActivation(&activation);
            engine.addListener(uptr.get());
            objects.push_back(std::move(uptr));
        } else if (obj.type == "clock_timer") {
            spiritClock.init(configuration, obj, spriteManager->get(obj.type));
            engine.addListener(&spiritClock);
        } else if (obj.type == "winged_foot") {
            jumpMeter.init(configuration, obj, spriteManager->get(obj.type));
            engine.addListener(&jumpMeter);
        } else if (obj.type == "zombie") {
            auto uptr = std::make_unique<PacingObject>();
            uptr->init(configuration, obj, spriteManager->get(obj.type));
            uptr->setActivation(&activation);
            engine.add(uptr.get());
            objects.push_back(std::move(uptr));
        } else if (obj.type == "rat") {
            auto uptr = std::make_unique<PacingObject>();
            uptr->init(configuration, obj, spriteManager->get(obj.type));
            uptr->setActivation(&activation);
            engine.add(uptr.get());
            objects.push_back(std::move(uptr));
        }
    }

    score.init(configuration, spriteManager->get("digits"));

    spirit.setPosition(-goggin.terminalVelocity.x * configuration.ticksPerSecond() * configuration.spiritSecondsBehind);
    spirit.setVelocity(goggin.terminalVelocity.x);

    spiritClock.setGoggin(&goggin);
    spiritClock.setSpirit(&spirit);

    jumpMeter.setGoggin(&goggin);

    activation.setProximity(static_cast<int>(configuration.activationProximity * scale->getMultiplier()));
    activation.setGoggin(&goggin);
}

trippin::Game::Game(std::string configName) : configName(std::move(configName)) {

}

trippin::Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void trippin::Game::start() {
    engine.start();
    renderLoop();
}

void trippin::Game::renderLoop() {
    Timer timer("renderer");
    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    goggin.onJumpCharge();
                }
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    goggin.onJumpRelease();
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 247, 251, 255, 255);
        SDL_RenderClear(renderer);

        goggin.center(camera);
        for (auto &obj : objects) {
            obj->render(camera);
        }

        goggin.render(camera);
        spiritClock.render(camera);
        jumpMeter.render(camera);
        score.render(camera);

        SDL_RenderPresent(renderer);

        timer.next();
    }
}