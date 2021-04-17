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
            if (!titleOverlay.hasScores() && stagingArea->bothSet()) {
                titleOverlay.setScores(stagingArea->getTodayScores(15), stagingArea->getTopScores(15));
            }
            titleOverlay.render();
            if (ui.anythingPressed()) {
                menuOverlay.reset();
                state = START_MENU;
            }
        } else if (state == START_MENU) {
            menuOverlay.render();
            if (menuOverlay.startClicked(ui.getLastPress())) {
                loadLevel = false;
                level->stop();
                level.reset();
                level = nextLevel();
                level->start();
                state = PLAYING;
            } else if (menuOverlay.exitClicked(ui.getLastPress())) {
                level->stop();
                break;
            } else if (menuOverlay.highScoreClicked(ui.getLastPress())) {
                state = SCORE_MENU;
                scoreMenuOverlay.reset();
            }
        } else if (state == SCORE_MENU) {
            scoreMenuOverlay.render();
            if (scoreMenuOverlay.exitClicked(ui.getLastPress())) {
                menuOverlay.reset();
                state = START_MENU;
            } else if (scoreMenuOverlay.allTimeClicked(ui.getLastPress())) {
                state = ALL_TIME_SCORES;
                allTimeScoresOverlay.reset();
                allTimeScoresOverlay.setScores(stagingArea->getTopScores(25));
            } else if (scoreMenuOverlay.todayClicked(ui.getLastPress())) {
                state = TODAY_SCORES;
                todayScoresOverlay.reset();
                todayScoresOverlay.setScores(stagingArea->getTodayScores(25));
            }
        } else if (state == ALL_TIME_SCORES) {
            allTimeScoresOverlay.render();
            if (ui.anythingPressed()) {
                menuOverlay.reset();
                state = START_MENU;
            }
        } else if (state == TODAY_SCORES) {
            todayScoresOverlay.render();
            if (ui.anythingPressed()) {
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
            if (endMenuOverlay.exitClicked(ui.getLastPress())) {
                menuOverlay.reset();
                state = START_MENU;
            } else if (endMenuOverlay.saveClicked(ui.getLastPress())) {
                state = NAME_FORM;
                nameFormOverlay.reset();
            }
        } else {
            nameFormOverlay.render();
            nameFormOverlay.onClick(ui.getLastPress());
            if (nameFormOverlay.nameEntered()) {
                stagingArea->addScore({score, rand(), nameFormOverlay.getName()});
                state = START_MENU;
                menuOverlay.reset();
            }
        }

        SDL_RenderPresent(sdlSystem->getRenderer());
        timer.next();
        ui.reset();
    }
}
