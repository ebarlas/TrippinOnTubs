#include <cstdlib>
#include <iostream>
#include <ctime>
#include "SDL_mixer.h"
#include "Game.h"
#include "net/DbSynchronizer.h"
#include "UserInput.h"

void trippin::Game::init() {
    initSdl();
    initRand();
    initConfiguration();
    initDbSychronizer();
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

void trippin::Game::initDbSychronizer() {
    stagingArea = std::make_shared<StagingArea>();
    Transport transport(configuration.db.host, configuration.db.port);
    DbSynchronizer::startAddScoresThread(transport, stagingArea);
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
            } else if (titleMenu->trainClicked(ui.getLastPress())) {
                loadLevel = false;
                trainLevel = true;
                score = 0;
                advanceLevel(score, extraLives);
                state = TRAINING;
            } else if (titleMenu->exitClicked(ui.getLastPress())) {
                level->stop();
                break;
            } else if (titleMenu->highScoreClicked(ui.getLastPress())) {
                state = SCORE_MENU;
                scoreMenu->reset();
            }
        } else if (state == SCORE_MENU) {
            scoreMenu->render();
            if (scoreMenu->exitClicked(ui.getLastPress())) {
                titleMenu->reset();
                state = START_MENU;
            } else if (scoreMenu->allTimeClicked(ui.getLastPress())) {
                state = ALL_TIME_SCORES;
                topScoreBoard->reset();
                topScoreBoard->setScores(stagingArea->getTopScores(25));
            } else if (scoreMenu->todayClicked(ui.getLastPress())) {
                state = TODAY_SCORES;
                todayScoreBoard->reset();
                todayScoreBoard->setScores(stagingArea->getTodayScores(25));
            }
        } else if (state == ALL_TIME_SCORES) {
            topScoreBoard->render();
            if (ui.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
            }
        } else if (state == TODAY_SCORES) {
            todayScoreBoard->render();
            if (ui.anythingPressed()) {
                titleMenu->reset();
                state = START_MENU;
            }
        } else if (state == PLAYING) {
            if (level->ended()) {
                score = level->getScore();
                if (level->completed()) {
                    state = LEVEL_TRANSITION;
                    levelOverlay->setLevel(levelIndex);
                } else {
                    if (extraLives > 0) {
                        extraLives--;
                        state = EXTRA_LIFE_DELAY;
                        extraLifeTime = SDL_GetTicks();
                    } else {
                        state = END_MENU;
                        endMenu->reset();
                    }
                }
            }
        } else if (state == EXTRA_LIFE_DELAY) {
            if (SDL_GetTicks() - extraLifeTime >= 1'000) {
                advanceLevel(score, extraLives);
                state = PLAYING;
            }
        } else if (state == TRAINING) {
            if (level->completed()) {
                loadLevel = true;
                trainLevel = false;
                score = 0;
                advanceLevel(score, extraLives);
                state = START_MENU;
            }
        } else if (state == LEVEL_TRANSITION) {
            levelOverlay->render();
            if (ui.anythingPressed()) {
                if (levelIndex < configuration.maps.size() - 1) {
                    levelIndex++;
                    advanceLevel(score, extraLives);
                    state = PLAYING;
                } else {
                    state = END_MENU;
                    endMenu->reset();
                }
            }
        } else if (state == END_MENU) {
            endMenu->render();
            if (endMenu->exitClicked(ui.getLastPress())) {
                titleMenu->reset();
                state = START_MENU;
            } else if (endMenu->saveClicked(ui.getLastPress())) {
                state = NAME_FORM;
                nameForm->reset();
            }
        } else {
            nameForm->render();
            nameForm->onClick(ui.getLastPress());
            if (nameForm->nameEntered()) {
                stagingArea->addScore({score, rand(), nameForm->getName()});
                state = START_MENU;
                titleMenu->reset();
            }
        }

        SDL_RenderPresent(sdlSystem->getRenderer());
        timer.next();
        ui.reset();
    }
}
