#include "SDL_mixer.h"
#include "Game.h"

void trippin::Game::init() {
    initRuntime();
    initWindowSize();
    initWindow();
    initRenderer();
    initMixer();
    initConfiguration();
    initScale();
    initSpriteManager();
    initAutoPlay();
    initLevel();
}

void trippin::Game::initRuntime() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL could not initialize. SDL error: %s", SDL_GetError());
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
        SDL_Log("Window could not be created. SDL error: %s", SDL_GetError());
        std::terminate();
    }
}

void trippin::Game::initRenderer() {
    renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created. SDL error: %s", SDL_GetError());
        std::terminate();
    }
}

void trippin::Game::initMixer() {
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

void trippin::Game::initConfiguration() {
    configuration.load(configName);
}

void trippin::Game::initScale() {
    auto &cs = configuration.scales;
    scale = &cs[0];
    for (auto it = cs.rbegin(); it != cs.rend(); it++) {
        if (windowSize.x >= it->minWidth) {
            scale = &(*it);
            break;
        }
    }
    SDL_Log("width=%d, height=%d, scale=%s", windowSize.x, windowSize.y, scale->name.c_str());
}

void trippin::Game::initSpriteManager() {
    auto sc = Scale{scale->name, scale->multiplier};
    spriteManager = std::make_unique<SpriteManager>(renderer, sc, configuration.tickPeriod);
}

void trippin::Game::initAutoPlay() {
    autoPlay.load(configuration.loadAutoPlay);
}

void trippin::Game::initLevel() {
    loadLevel = true;
    level = nextLevel();
}

std::unique_ptr<trippin::Level> trippin::Game::nextLevel() {
    auto lvl = std::make_unique<Level>();
    lvl->setWindowSize(windowSize);
    lvl->setConfiguration(&configuration);
    lvl->setScale(scale);
    lvl->setSpriteManager(spriteManager.get());
    lvl->setSoundManager(&soundManager);
    if (loadLevel) {
        lvl->setMapName(configuration.loadMap);
        lvl->setAutoPlay(autoPlay.events);
    } else {
        lvl->setMapName(configuration.map);
    }
    lvl->init();
    return lvl;
}

trippin::Game::Game(std::string configName) : configName(std::move(configName)) {

}

trippin::Game::~Game() {
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void trippin::Game::start() {
    level->start();
    renderLoop();
}

void trippin::Game::renderLoop() {
    auto overlay = spriteManager->get("trippin");

    Point<int> overlayPos;
    overlayPos.x = (windowSize.x - overlay.getSize().x) / 2;
    overlayPos.y = (windowSize.y - overlay.getSize().y) / 2;

    Timer timer("renderer");
    bool quit = false;
    while (!quit) {
        SDL_Event e;
        UserInput input{};
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    input.jumpCharge = true;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    input.duckStart = true;
                }
            } else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    input.jumpRelease = true;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    input.duckEnd = true;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 247, 251, 255, 255);
        SDL_RenderClear(renderer);

        if (loadLevel) {
            if (input.jumpRelease) {
                level->stop();
                level.reset();
                loadLevel = false;
                level = nextLevel();
                level->start();
            } else {
                level->render(input);
                overlay.render(overlayPos, 0);
            }
        } else {
            level->render(input);
        }

        SDL_RenderPresent(renderer);
        timer.next();
    }
}
