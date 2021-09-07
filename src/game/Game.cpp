#include <cstdlib>
#include <iostream>
#include <sstream>
#include <ctime>
#include "SDL_syswm.h"
#include "SDL_mixer.h"
#include "Game.h"
#include "net/DbSynchronizer.h"
#include "UserInput.h"

void trippin::Game::init() {
    initSdl();
    initRand();
    initConfiguration();
    initDbSychronizer();
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
    auto ws = sdlSystem->getWindowSize();
    windowSize = {ws.x, ws.y};
    auto rs = sdlSystem->getRendererSize();
    rendererSize = {rs.x, rs.y};
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
    ss << "op=init, rendererName=" << renName << ", systemName=" << sysName << ", systemRam=" << sysRam;
    logger->log(ss.str());
}

void trippin::Game::initDbSychronizer() {
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
    spriteManager = std::make_unique<SpriteManager>(sdlSystem->getRenderer(), *spriteLoader, configuration.tickPeriod);
}

void trippin::Game::initAutoPlay() {
    autoPlay.load(configuration.loadAutoPlay);
}

void trippin::Game::initLevel() {
    levelIndex = 0;
    loadLevel = true;
    trainLevel = false;
    level = nextLevel();
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

std::unique_ptr<trippin::Level> trippin::Game::nextLevel() {
    auto lvl = std::make_unique<Level>();
    lvl->setWindowSize(rendererSize);
    lvl->setConfiguration(&configuration);
    lvl->setScale(scale);
    lvl->setSpriteManager(spriteManager.get());
    lvl->setSoundManager(&soundManager);
    lvl->setRenderClock(renderClock);
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
    level = nextLevel();
    level->setScore(score);
    level->setExtraLives(extraLives);
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
    Uint32 extraLifeTime;

    Timer timer("renderer");
    UserInput ui;
    while (true) {
        ui.pollEvents();

        if (ui.quitPressed()) {
            level->stop();
            break;
        }

        if (!renderClock.isPaused() && (ui.pPressed() || ui.wasFocusLost())) {
            renderClock.pause();
            level->pause();
        }
        if (renderClock.isPaused() && (ui.rPressed() || ui.wasFocusGained())) {
            renderClock.resume();
            level->resume();
        }
        if (renderClock.isPaused()) {
            SDL_Delay(1);
            continue;
        }

        renderClock.update();

        SDL_SetRenderDrawColor(sdlSystem->getRenderer(), 244, 251, 255, 255);
        SDL_RenderClear(sdlSystem->getRenderer());

        level->render(ui.asGogginInput());

        if (state == TITLE) {
            if (!titleOverlay->hasScores() && stagingArea->bothSet()) {
                titleOverlay->setScores(stagingArea->getTodayScores(15), stagingArea->getTopScores(15));
            }
            titleOverlay->render();
            if (ui.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("TITLE", "START_MENU");
            }
        } else if (state == START_MENU) {
            titleMenu->render();
            if (titleMenu->startClicked(ui.getLastPress())) {
                extraLives = 1;
                loadLevel = false;
                trainLevel = false;
                score = 0;
                advanceLevel(score, extraLives);
                state = PLAYING;
                logStateChange("START_MENU", "PLAYING");
            } else if (titleMenu->trainClicked(ui.getLastPress())) {
                loadLevel = false;
                trainLevel = true;
                score = 0;
                advanceLevel(score, extraLives);
                state = TRAINING;
                logStateChange("START_MENU", "TRAINING");
            } else if (titleMenu->exitClicked(ui.getLastPress())) {
                level->stop();
                break;
            } else if (titleMenu->highScoreClicked(ui.getLastPress())) {
                state = SCORE_MENU;
                scoreMenu->reset();
                logStateChange("START_MENU", "SCORE_MENU");
            }
        } else if (state == SCORE_MENU) {
            scoreMenu->render();
            if (scoreMenu->exitClicked(ui.getLastPress())) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("SCORE_MENU", "START_MENU");
            } else if (scoreMenu->allTimeClicked(ui.getLastPress())) {
                state = ALL_TIME_SCORES;
                topScoreBoard->reset();
                topScoreBoard->setScores(stagingArea->getTopScores(25));
                logStateChange("SCORE_MENU", "ALL_TIME_SCORES");
            } else if (scoreMenu->todayClicked(ui.getLastPress())) {
                state = TODAY_SCORES;
                todayScoreBoard->reset();
                todayScoreBoard->setScores(stagingArea->getTodayScores(25));
                logStateChange("SCORE_MENU", "TODAY_SCORES");
            }
        } else if (state == ALL_TIME_SCORES) {
            topScoreBoard->render();
            if (ui.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("ALL_TIME_SCORES", "START_MENU");
            }
        } else if (state == TODAY_SCORES) {
            todayScoreBoard->render();
            if (ui.anythingPressed()) {
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
                        extraLifeTime = SDL_GetTicks();
                        logStateChange("PLAYING", "EXTRA_LIFE_DELAY");
                    } else {
                        state = END_MENU;
                        endMenu->reset();
                        logStateChange("PLAYING", "END_MENU");
                    }
                }
            }
        } else if (state == EXTRA_LIFE_DELAY) {
            if (SDL_GetTicks() - extraLifeTime >= 1'000) {
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
            levelOverlay->render();
            if (ui.anythingPressed()) {
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
            endMenu->render();
            if (endMenu->exitClicked(ui.getLastPress())) {
                titleMenu->reset();
                state = START_MENU;
                logStateChange("END_MENU", "START_MENU");
            } else if (endMenu->saveClicked(ui.getLastPress())) {
                state = NAME_FORM;
                nameForm->reset();
                logStateChange("END_MENU", "NAME_FORM");
            }
        } else {
            nameForm->render();
            nameForm->onClick(ui.getLastPress());
            if (nameForm->nameEntered()) {
                stagingArea->addScore({score, rand(), nameForm->getName()});
                state = START_MENU;
                titleMenu->reset();
                logStateChange("NAME_FORM", "START_MENU");
            }
        }

        SDL_RenderPresent(sdlSystem->getRenderer());
        timer.next();
        ui.reset();
    }
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
