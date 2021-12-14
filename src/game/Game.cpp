#include <cstdlib>
#include <iostream>
#include <ctime>
#include <map>
#include "SDL_syswm.h"
#include "SDL_mixer.h"
#include "Game.h"
#include "net/DbSynchronizer.h"
#include "UserInput.h"

void trippin::Game::init() {
    initSdl();
    initRand();
    initConfiguration();
    initDbSynchronizer();
    initLogger();
    initScale();
    initSpriteManager();
    initAutoPlay();
    initOverlays();
    initLevel();
    initClock();
}

void trippin::Game::initSdl() {
    sdlSystem = std::make_unique<SdlSystem>();
    windowSize = convertPoint(sdlSystem->getWindowSize());
    rendererSize = convertPoint(sdlSystem->getRendererSize());
}

void trippin::Game::initRand() {
    std::srand(std::time(0));
    SDL_Log("seeded random number generator, RAND_MAX=%d", RAND_MAX);
}

void trippin::Game::initLogger() {
    logger = std::make_unique<Logger>(*stagingArea);
    auto renName = getRendererName(sdlSystem->getRenderer());
    auto sysName = getSystemName(sdlSystem->getWindow());
    auto sysRam = SDL_GetSystemRAM();
    std::stringstream ss;
    ss << "op=init";
    ss << ", rendererName=" << renName;
    ss << ", systemName=" << sysName;
    ss << ", systemRam=" << sysRam;
    ss << ", winSize=(" << windowSize.x << "," << windowSize.y << ")";
    ss << ", renSize=(" << rendererSize.x << "," << rendererSize.y << ")";
    logger->log(ss.str());
}

void trippin::Game::initDbSynchronizer() {
    stagingArea = std::make_shared<StagingArea>();
    Transport transport(configuration.db.host, configuration.db.port);
    DbSynchronizer::startAddScoresThread(transport, stagingArea);
    DbSynchronizer::startAddLogEventsThread(transport, stagingArea);
    DbSynchronizer::startQueryScoresThread(std::move(transport), stagingArea);
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
    spriteLoader = std::make_unique<SpriteLoader>(Scale{scale->name, scale->multiplier});
    spriteLoadTask = std::make_unique<SpriteLoadTask>(*spriteLoader, configuration.prefetchSprites);
    spriteManager = std::make_unique<SpriteManager>(sdlSystem->getRenderer(), *spriteLoader, configuration.msPerTick());
}

void trippin::Game::initAutoPlay() {
    autoPlay.load(configuration.loadAutoPlay);
}

void trippin::Game::initLevel() {
    levelIndex = 0;
    loadLevel = true;
    trainLevel = false;
    level = nextLevel(0, 1);
    spriteLoadTask->start();
}

void trippin::Game::initOverlays() {
    TitleOverlay::Options titleOptions{-0.25, 3'000};
    titleOverlay = std::make_unique<TitleOverlay>(rendererSize, titleOptions, *spriteManager, renderClock);
    titleMenu = std::make_unique<TitleMenu>(rendererSize, *spriteManager, renderClock);
    endMenu = std::make_unique<EndMenu>(rendererSize, *spriteManager, renderClock);
    nameForm = std::make_unique<NameForm>(rendererSize, *spriteManager);
    scoreMenu = std::make_unique<ScoreMenu>(rendererSize, *spriteManager, renderClock);
    topScoreBoard = std::make_unique<ScrollingScoreBoard>(rendererSize, -0.25, *spriteManager, renderClock);
    todayScoreBoard = std::make_unique<ScrollingScoreBoard>(rendererSize, -0.25, *spriteManager, renderClock);
    levelOverlay = std::make_unique<LevelOverlay>(rendererSize, *spriteManager, renderClock);
}

void trippin::Game::initClock() {
    renderClock.init();
}

std::unique_ptr<trippin::Level> trippin::Game::nextLevel(int score, int extraLives) {
    auto lvl = std::make_unique<Level>();
    lvl->setWindowSize(rendererSize);
    lvl->setConfiguration(&configuration);
    lvl->setScale(scale);
    lvl->setSpriteManager(spriteManager.get());
    lvl->setSoundManager(&soundManager);
    lvl->setRenderClock(renderClock);
    lvl->setScore(score);
    lvl->setExtraLives(extraLives);
    if (loadLevel) {
        lvl->setMapName(configuration.loadMap);
        lvl->setAutoPlay(autoPlay.events);
    } else if (trainLevel) {
        lvl->setMapName(configuration.trainMap);
        lvl->setTraining(true);
    } else {
        lvl->setMapName(configuration.maps[levelIndex]);
        transferSurfaces();
    }
    lvl->init();
    return lvl;
}

void trippin::Game::advanceLevel(int score, int extraLives) {
    level->stop();
    level.reset();
    level = nextLevel(score, extraLives);
    level->start();
}

void trippin::Game::transferSurfaces() {
    if (spriteLoadTask->started() && !spriteLoadTask->joined()) {
        spriteLoadTask->join();
        spriteManager->setSurfaces(spriteLoadTask->take());
    }
}

trippin::Game::Game(std::string configName) : configName(std::move(configName)) {

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
        TRAINING,
        PLAYING,
        EXTRA_LIFE_DELAY,
        LEVEL_TRANSITION,
        END_MENU,
        NAME_FORM,
        ALL_TIME_SCORES,
        TODAY_SCORES,
    };

    int state = TITLE;
    int score;
    int extraLives = 1;
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> extraLifeTime;

    int lastTicks = 0;

    std::map<int, int> ticksPerFrame;

    auto timerFn = [&ticksPerFrame](int tps) {
        SDL_Log("timer=renderer, fps=%d, tpf=%s", tps, format(ticksPerFrame).c_str());
        ticksPerFrame.clear();
    };

    Timer timer(timerFn);
    UserInput ui;
    while (true) {
        auto event = ui.pollEvent();
        if (!event) {
            continue;
        }

        if (event.quit) {
            level->stop();
            break;
        }

        auto gogginInput = event.asGogginInput();
        if (gogginInput) {
            level->onInput(gogginInput);
        }

        if (!renderClock.isPaused() && (event.pKeyDown || event.focusLost)) {
            renderClock.pause();
            level->pause();
        }
        if (renderClock.isPaused() && (event.rKeyDown || event.focusGained)) {
            renderClock.resume();
            level->resume();
        }
        if (renderClock.isPaused()) {
            std::this_thread::sleep_for(std::chrono::milliseconds{1});
            continue;
        }

        if (state == TITLE) {
            if (!titleOverlay->hasScores() && stagingArea->bothSet()) {
                titleOverlay->setScores(stagingArea->getTodayScores(15), stagingArea->getTopScores(15));
            }
            if (event.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("TITLE", "START_MENU");
            }
        } else if (state == START_MENU) {
            if (titleMenu->startClicked(event.touchPoint)) {
                extraLives = 1;
                loadLevel = false;
                trainLevel = false;
                score = 0;
                advanceLevel(score, extraLives);
                state = PLAYING;
                logStateChange("START_MENU", "PLAYING");
            } else if (titleMenu->trainClicked(event.touchPoint)) {
                loadLevel = false;
                trainLevel = true;
                score = 0;
                advanceLevel(score, extraLives);
                state = TRAINING;
                logStateChange("START_MENU", "TRAINING");
            } else if (titleMenu->exitClicked(event.touchPoint)) {
                level->stop();
                break;
            } else if (titleMenu->highScoreClicked(event.touchPoint)) {
                state = SCORE_MENU;
                scoreMenu->reset();
                logStateChange("START_MENU", "SCORE_MENU");
            }
        } else if (state == SCORE_MENU) {
            if (scoreMenu->exitClicked(event.touchPoint)) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("SCORE_MENU", "START_MENU");
            } else if (scoreMenu->allTimeClicked(event.touchPoint)) {
                state = ALL_TIME_SCORES;
                topScoreBoard->reset();
                topScoreBoard->setScores(stagingArea->getTopScores(25));
                logStateChange("SCORE_MENU", "ALL_TIME_SCORES");
            } else if (scoreMenu->todayClicked(event.touchPoint)) {
                state = TODAY_SCORES;
                todayScoreBoard->reset();
                todayScoreBoard->setScores(stagingArea->getTodayScores(25));
                logStateChange("SCORE_MENU", "TODAY_SCORES");
            }
        } else if (state == ALL_TIME_SCORES) {
            if (event.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("ALL_TIME_SCORES", "START_MENU");
            }
        } else if (state == TODAY_SCORES) {
            if (event.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("TODAY_SCORES", "START_MENU");
            }
        } else if (state == PLAYING) {
            if (level->ended()) {
                score = level->getScore();
                if (level->completed()) {
                    state = LEVEL_TRANSITION;
                    levelOverlay->setLevel(levelIndex);
                    logStateChange("PLAYING", "LEVEL_TRANSITION");
                } else {
                    if (extraLives > 0) {
                        extraLives--;
                        state = EXTRA_LIFE_DELAY;
                        auto now = std::chrono::steady_clock::now();
                        extraLifeTime = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
                        logStateChange("PLAYING", "EXTRA_LIFE_DELAY");
                    } else {
                        state = END_MENU;
                        endMenu->reset();
                        logStateChange("PLAYING", "END_MENU");
                    }
                }
            }
        } else if (state == EXTRA_LIFE_DELAY) {
            auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
            if (now - extraLifeTime >= std::chrono::seconds{1}) {
                advanceLevel(score, extraLives);
                state = PLAYING;
                logStateChange("EXTRA_LIFE_DELAY", "PLAYING");
            }
        } else if (state == TRAINING) {
            if (level->completed()) {
                loadLevel = true;
                trainLevel = false;
                score = 0;
                advanceLevel(score, extraLives);
                state = START_MENU;
                logStateChange("TRAINING", "START_MENU");
            }
        } else if (state == LEVEL_TRANSITION) {
            if (event.anythingPressed()) {
                if (levelIndex < configuration.maps.size() - 1) {
                    levelIndex++;
                    advanceLevel(score, extraLives);
                    state = PLAYING;
                    logStateChange("LEVEL_TRANSITION", "PLAYING");
                } else {
                    state = END_MENU;
                    endMenu->reset();
                    logStateChange("LEVEL_TRANSITION", "END_MENU");
                }
            }
        } else if (state == END_MENU) {
            if (endMenu->exitClicked(event.touchPoint)) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("END_MENU", "START_MENU");
            } else if (endMenu->saveClicked(event.touchPoint)) {
                state = NAME_FORM;
                nameForm->reset();
                logStateChange("END_MENU", "NAME_FORM");
            }
        } else {
            nameForm->onClick(event.touchPoint);
            if (nameForm->nameEntered()) {
                stagingArea->addScore({score, rand(), nameForm->getName()});
                state = START_MENU;
                titleMenu->reset();
                logStateChange("NAME_FORM", "START_MENU");
            }
        }

        if (event.render) {
            renderClock.update();

            SDL_SetRenderDrawColor(sdlSystem->getRenderer(), 244, 251, 255, 255);
            SDL_RenderClear(sdlSystem->getRenderer());

            level->render();

            if (state == TITLE) {
                titleOverlay->render();
            } else if (state == START_MENU) {
                titleMenu->render();
            } else if (state == SCORE_MENU) {
                scoreMenu->render();
            } else if (state == ALL_TIME_SCORES) {
                topScoreBoard->render();
            } else if (state == TODAY_SCORES) {
                todayScoreBoard->render();
            } else if (state == LEVEL_TRANSITION) {
                levelOverlay->render();
            } else if (state == END_MENU) {
                endMenu->render();
            } else if (state == NAME_FORM) {
                nameForm->render();
            }

            SDL_RenderPresent(sdlSystem->getRenderer());

            auto ticks = level->getTicks();
            auto diff = ticks - lastTicks;
            lastTicks = ticks;
            ticksPerFrame[diff]++;

            timer.next();
        }
    }
}

std::string trippin::Game::format(const std::map<int, int> &map) {
    std::stringstream ss;
    ss << "[";
    for (auto i = map.begin(); i != map.end();) {
        auto cur = i++;
        ss << cur->first << "=" << cur->second;
        if (i != map.end()) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

const char *trippin::Game::getSystemName(SDL_Window *window) {
    SDL_SysWMinfo info;

    // snippet from https://wiki.libsdl.org/SDL_GetWindowWMInfo
    SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(window, &info)) {
        switch (info.subsystem) {
            case SDL_SYSWM_WINDOWS:
                return "MicrosoftWindows";
            case SDL_SYSWM_X11:
                return "XWindowSystem";
            case SDL_SYSWM_WINRT:
                return "WinRT";
            case SDL_SYSWM_DIRECTFB:
                return "DirectFB";
            case SDL_SYSWM_COCOA:
                return "AppleOSX";
            case SDL_SYSWM_UIKIT:
                return "UIKit";
            case SDL_SYSWM_WAYLAND:
                return "Wayland";
            case SDL_SYSWM_MIR:
                return "Mir";
            case SDL_SYSWM_ANDROID:
                return "Android";
            case SDL_SYSWM_VIVANTE:
                return "Vivante";
        }
    }

    return "unknown";
}

const char *trippin::Game::getRendererName(SDL_Renderer *renderer) {
    SDL_RendererInfo ri;
    if (SDL_GetRendererInfo(renderer, &ri) == 0) {
        return ri.name;
    }
    return "unknown";
}

void trippin::Game::logStateChange(const char *prev, const char *next) {
    using namespace std::string_literals;
    logger->log("op=state_change, prev="s + prev + ", next=" + next);
}
