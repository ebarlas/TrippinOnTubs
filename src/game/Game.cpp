#include <cstdlib>
#include <iostream>
#include <ctime>
#include "SDL_mixer.h"
#include "Game.h"
#include "net/DbSynchronizer.h"

void trippin::Game::init() {
    initRuntime();
    initWindowSize();
    initWindow();
    initRenderer();
    initMixer();
    initRand();
    initConfiguration();
    initDbSychronizer();
    initScale();
    initSpriteManager();
    initAutoPlay();
    initOverlays();
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

void trippin::Game::initRand() {
    std::srand(std::time(0));
    SDL_Log("seeded random number generator, RAND_MAX=%d", RAND_MAX);
}

void trippin::Game::initDbSychronizer() {
    // intentional use of new operator
    // object duration is now until termination
    // these objects are accessed by detached background threads
    stagingArea = new StagingArea;
    auto transport = new Transport(configuration.db.host, configuration.db.port);
    auto sync = new DbSynchronizer(*transport, *stagingArea);
    sync->start();
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

void trippin::Game::initOverlays() {
    titleOverlay.setTitlePause(3'000);
    titleOverlay.setScrollRate(-0.25);
    titleOverlay.init(windowSize, *spriteManager);
    menuOverlay.init(windowSize, *spriteManager);
    endMenuOverlay.init(windowSize, *spriteManager);
    nameFormOverlay.init(windowSize, *spriteManager);
    scoreMenuOverlay.init(windowSize, *spriteManager);
    allTimeScoresOverlay.setScrollRate(-0.25);
    allTimeScoresOverlay.init(windowSize, *spriteManager);
    todayScoresOverlay.setScrollRate(-0.25);
    todayScoresOverlay.init(windowSize, *spriteManager);
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
    enum State {
        TITLE,
        START_MENU,
        SCORE_MENU,
        PLAYING,
        END_MENU,
        NAME_FORM,
        ALL_TIME_SCORES,
        TODAY_SCORES,
    };

    int state = TITLE;
    int score;

    Timer timer("renderer");
    UserInput ui{};
    while (!ui.quit) {
        ui = pollEvents();

        SDL_SetRenderDrawColor(renderer, 244, 251, 255, 255);
        SDL_RenderClear(renderer);

        level->render(getGogginInput(ui));

        if (state == TITLE) {
            if (!titleOverlay.hasScores() && stagingArea->bothSet()) {
                titleOverlay.setScores(stagingArea->getTodayScores(15), stagingArea->getTopScores(15));
            }
            titleOverlay.render();
            if (ui.spaceKeyUp) {
                menuOverlay.reset();
                state = START_MENU;
            }
        } else if (state == START_MENU) {
            menuOverlay.render();
            if (ui.mouseButtonDown && menuOverlay.startClicked(ui.mouseButton)) {
                loadLevel = false;
                level->stop();
                level.reset();
                level = nextLevel();
                level->start();
                state = PLAYING;
            } else if (ui.mouseButtonDown && menuOverlay.exitClicked(ui.mouseButton)) {
                level->stop();
                break;
            } else if (ui.mouseButtonDown && menuOverlay.highScoreClicked(ui.mouseButton)) {
                state = SCORE_MENU;
                scoreMenuOverlay.reset();
            }
        } else if (state == SCORE_MENU) {
            scoreMenuOverlay.render();
            if (ui.mouseButtonDown && scoreMenuOverlay.exitClicked(ui.mouseButton)) {
                menuOverlay.reset();
                state = START_MENU;
            } else if (ui.mouseButtonDown && scoreMenuOverlay.allTimeClicked(ui.mouseButton)) {
                state = ALL_TIME_SCORES;
                allTimeScoresOverlay.reset();
                allTimeScoresOverlay.setScores(stagingArea->getTopScores(25));
            } else if (ui.mouseButtonDown && scoreMenuOverlay.todayClicked(ui.mouseButton)) {
                state = TODAY_SCORES;
                todayScoresOverlay.reset();
                todayScoresOverlay.setScores(stagingArea->getTodayScores(25));
            }
        } else if (state == ALL_TIME_SCORES) {
            allTimeScoresOverlay.render();
            if (ui.spaceKeyUp) {
                menuOverlay.reset();
                state = START_MENU;
            }
        } else if (state == TODAY_SCORES) {
            todayScoresOverlay.render();
            if (ui.spaceKeyUp) {
                menuOverlay.reset();
                state = START_MENU;
            }
        } else if (state == PLAYING) {
            if (level->ended()) {
                score = level->getScore();
                state = END_MENU;
                endMenuOverlay.reset();
            }
        } else if (state == END_MENU) {
            endMenuOverlay.render();
            if (ui.mouseButtonDown && endMenuOverlay.exitClicked(ui.mouseButton)) {
                menuOverlay.reset();
                state = START_MENU;
            } else if (ui.mouseButtonDown && endMenuOverlay.saveClicked(ui.mouseButton)) {
                state = NAME_FORM;
                nameFormOverlay.reset();
            }
        } else {
            nameFormOverlay.render();
            if (ui.mouseButtonDown) {
                nameFormOverlay.onClick(ui.mouseButton);
                if (nameFormOverlay.nameEntered()) {
                    stagingArea->addScore({score, nameFormOverlay.getName(), std::to_string(rand())});
                    state = START_MENU;
                    menuOverlay.reset();
                }
            }
        }

        SDL_RenderPresent(renderer);
        timer.next();
    }
}

trippin::Game::UserInput trippin::Game::pollEvents() {
    SDL_Event e;
    UserInput ui{};
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            ui.quit = true;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                ui.spaceKeyDown = true;
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                ui.downKeyDown = true;
            }
        } else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                ui.spaceKeyUp = true;
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                ui.downKeyUp = true;
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            ui.mouseButtonDown = true;
            ui.mouseButton = {e.button.x, e.button.y};
        }
    }
    return ui;
}

trippin::GogginInput trippin::Game::getGogginInput(const UserInput &ui) {
    GogginInput gi;
    gi.jumpCharge = ui.spaceKeyDown;
    gi.jumpRelease = ui.spaceKeyUp;
    gi.duckStart = ui.downKeyDown;
    gi.duckEnd = ui.downKeyUp;
    return gi;
}
