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
}

void trippin::Game::initSdl() {
    sdlSystem = std::make_unique<SdlSystem>();
    auto ws = sdlSystem->getWindowSize();
    windowSize = {ws.x, ws.y};
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
    auto sc = Scale{scale->name, scale->multiplier};
    spriteManager = std::make_unique<SpriteManager>(sdlSystem->getRenderer(), sc, configuration.tickPeriod);
}

void trippin::Game::initAutoPlay() {
    autoPlay.load(configuration.loadAutoPlay);
}

void trippin::Game::initLevel() {
    levelIndex = 0;
    loadLevel = true;
    level = nextLevel();
}

void trippin::Game::initOverlays() {
    TitleOverlay::Options titleOptions{-0.25, 3'000};
    titleOverlay = std::make_unique<TitleOverlay>(windowSize, titleOptions, *spriteManager);
    titleMenu = std::make_unique<TitleMenu>(windowSize, *spriteManager);
    endMenu = std::make_unique<EndMenu>(windowSize, *spriteManager);
    nameForm = std::make_unique<NameForm>(windowSize, *spriteManager);
    scoreMenu = std::make_unique<ScoreMenu>(windowSize, *spriteManager);
    topScoreBoard = std::make_unique<ScrollingScoreBoard>(windowSize, -0.25, *spriteManager);
    todayScoreBoard = std::make_unique<ScrollingScoreBoard>(windowSize, -0.25, *spriteManager);
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
        lvl->setMapName(configuration.maps[levelIndex]);
    }
    lvl->init();
    return lvl;
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
        PLAYING,
        LEVEL_TRANSITION,
        END_MENU,
        NAME_FORM,
        ALL_TIME_SCORES,
        TODAY_SCORES,
    };

    int state = TITLE;
    int score;
    Uint32 levelTransitionStart;

    Timer timer("renderer");
    UserInput ui;
    while (!ui.quitPressed()) {
        ui.pollEvents();

        SDL_SetRenderDrawColor(sdlSystem->getRenderer(), 244, 251, 255, 255);
        SDL_RenderClear(sdlSystem->getRenderer());

        if (ui.pPressed()) {
            level->pause();
        }
        if (ui.rPressed()) {
            level->resume();
        }

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
                loadLevel = false;
                level->stop();
                level.reset();
                level = nextLevel();
                level->start();
                state = PLAYING;
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
                if (level->completed() && levelIndex < configuration.maps.size() - 1) {
                    state = LEVEL_TRANSITION;
                    levelTransitionStart = SDL_GetTicks();
                } else {
                    state = END_MENU;
                    endMenu->reset();
                }
            }
        } else if (state == LEVEL_TRANSITION) {
            if (SDL_GetTicks() - levelTransitionStart > 3'000) {
                levelIndex++;
                level->stop();
                level.reset();
                level = nextLevel();
                level->setScore(score);
                level->start();
                state = PLAYING;
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
