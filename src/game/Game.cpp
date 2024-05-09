#include <iostream>
#include <sstream>
#include <map>
#include <chrono>
#include "SDL_syswm.h"
#include "SDL_mixer.h"
#include "Game.h"
#include "UserInput.h"

void trippin::Game::init() {
    initConfiguration();
    initSdl();
    initAppId();
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
    sdlSystem = std::make_unique<SdlSystem>(SDL_Point{configuration.windowSize.x, configuration.windowSize.y});
    windowSize = convertPoint(sdlSystem->getWindowSize());
    rendererSize = convertPoint(sdlSystem->getRendererSize());
}

void trippin::Game::initAppId() {
    auto now = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    std::stringstream ss;
    ss << "T";
    ss << dur;
    ss << "N";
    ss << random.next();

    appId = ss.str();
    gameId = 0;

    SDL_Log("appId=%s", appId.c_str());
}

void trippin::Game::initLogger() {
    logger = std::make_unique<Logger>([this](const LogEvent &e) { logDb->add(e); }, appId);
    SDL_DisplayMode displayMode;
    SDL_GetDisplayMode(0, 0, &displayMode);
    auto platform = SDL_GetPlatform();
    auto renName = getRendererName(sdlSystem->getRenderer());
    auto sysName = getSystemName(sdlSystem->getWindow());
    auto sysRam = SDL_GetSystemRAM();
    std::stringstream ss;
    ss << "op=init";
    ss << ", version=" << configuration.version.major << "." << configuration.version.minor;
    ss << ", platform=" << platform;
    ss << ", rendererName=" << renName;
    ss << ", systemName=" << sysName;
    ss << ", systemRam=" << sysRam;
    ss << ", frameRate=" << displayMode.refresh_rate;
    ss << ", highDpiScale=" << sdlSystem->getHighDpiScale();
    ss << ", winSize=(" << windowSize.x << "," << windowSize.y << ")";
    logger->log(ss.str());
}

namespace trippin {
    void to_json(nlohmann::json &j, const trippin::Db<Score>::Event &event) {
        j["val"] = event.val;
        j["add"] = event.add;
    }

    void from_json(const nlohmann::json &j, trippin::Db<Score>::Event &event) {
        j.at("val").get_to(event.val);
        j.at("add").get_to(event.add);
    }

    void to_json(nlohmann::json &j, const trippin::Db<LogEvent>::Event &event) {
        j["val"] = event.val;
        j["add"] = event.add;
    }

    void from_json(const nlohmann::json &j, trippin::Db<LogEvent>::Event &event) {
        j.at("val").get_to(event.val);
        j.at("add").get_to(event.add);
    }
};

void trippin::Game::initDbSynchronizer() {
    int version = configuration.version.major;
    transport = std::make_unique<Transport>(
            configuration.db.host,
            configuration.db.port,
            version,
            configuration.highScores);
    stagingArea = std::make_unique<StagingArea>(*transport);
    stagingArea->start();
    myScores = std::make_unique<MyScores>(version, 10);
    auto scoreAddCallback = [m = myScores.get()](const Score &s) {
        m->addScore(s);
    };
    auto scoreDispatchFn = [t = transport.get()](const Score &s) {
        auto result = t->addScore(s);
        SDL_Log("add score attempted, id=%s, game=%d, name=%s, score=%d, result=%s",
                s.id.c_str(), s.game, s.name.c_str(), s.score, toString(result));
        return result == AddResult::success || result == AddResult::clientError;
    };
    int mdcs = configuration.maxDispatchChannelSize;
    scoreDb = std::make_unique<Db<Score>>("scores", version, mdcs, scoreAddCallback, scoreDispatchFn);
    scoreDb->start();
    auto logAddCallback = [](const LogEvent &e) {};
    auto logDispatchFn = [t = transport.get()](const LogEvent &e) {
        auto result = t->addLogEvent(e);
        SDL_Log("add log event attempted, index=%d, result=%s", e.index, toString(result));
        return result == AddResult::success || result == AddResult::clientError;
    };
    logDb = std::make_unique<Db<LogEvent>>("logs", version, mdcs, logAddCallback, logDispatchFn);
    logDb->start();
}

void trippin::Game::initConfiguration() {
    configuration.load(configName);
}

void trippin::Game::initScale() {
    auto &cs = configuration.scales;
    auto sc = &cs[0];
    for (auto it = cs.rbegin(); it != cs.rend(); it++) {
        if (windowSize.x >= it->minWidth) {
            sc = &(*it);
            break;
        }
    }
    // using LCM to derive the engine factor ensures that conversions between base, device, and engine
    // scales in any direction always occur with whole numbers and facilitates simple int arithmetic
    auto lcm = std::accumulate(
            configuration.scales.begin(),
            configuration.scales.end(),
            1,
            [](int a, const Configuration::Scale &b) { return std::lcm(a, b.multiplier); });
    scale = std::make_unique<Scale>(sc->name, sc->multiplier, lcm);
    SDL_Log("width=%d, height=%d, scale=%s, deviceFactor=%d, engineFactor=%d, deviceEngineFactor=%d",
            windowSize.x, windowSize.y, sc->name.c_str(), sc->multiplier, lcm, scale->getDeviceEngineFactor());
    configuration.rescale(*scale);
}

void trippin::Game::initSpriteManager() {
    spriteLoader = std::make_unique<SpriteLoader>(*scale);
    spriteLoadTask = std::make_unique<SpriteLoadTask>(*spriteLoader, configuration.prefetchSprites);
    spriteManager = std::make_unique<SpriteManager>(sdlSystem->getRenderer(), *spriteLoader, configuration.msPerTick());
    auto digitSize = spriteManager->get("digits").getDeviceSize();
    auto textWidth = digitSize.x * 4;
    auto margin = configuration.meterMargin;
    scoreArea = Rect<int>{windowSize.x / 2 - textWidth / 2, margin, textWidth, digitSize.y};
    auto flagsSize = spriteManager->get("training_status").getDeviceSize();
    flagsArea = Rect<int>{windowSize.x - flagsSize.x - margin, margin, flagsSize.x, flagsSize.y};
}

void trippin::Game::initAutoPlay() {
    autoPlay.load(configuration.loadAutoPlay);
}

void trippin::Game::initLevel() {
    state = State::TITLE;
    levelIndex = 0;
    level = nextLevel();
    spriteLoadTask->start();
}

void trippin::Game::initOverlays() {
    auto makeOverlay = [this](const char *name) {
        return std::make_unique<SimpleOverlay>(rendererSize, spriteManager->get(name), renderClock);
    };
    auto scrollPxPerMs = -configuration.scrollPixelsPerSecond / 1'000.0;
    TitleOverlay::Options titleOptions{scrollPxPerMs, 3'000};
    titleOverlay = std::make_unique<TitleOverlay>(rendererSize, titleOptions, *spriteManager, renderClock);
    titleMenu = std::make_unique<MenuLayout<4>>(
            rendererSize,
            750,
            renderClock,
            std::array<const Sprite *, 4>{
                    &spriteManager->get("start"),
                    &spriteManager->get("train"),
                    &spriteManager->get("high_score"),
                    &spriteManager->get("my_scores")});
    endMenu = std::make_unique<MenuLayout<2>>(
            rendererSize,
            750,
            renderClock,
            std::array<const Sprite *, 2>{
                    &spriteManager->get("save_score"),
                    &spriteManager->get("exit")});
    nameForm = std::make_unique<NameForm>(rendererSize, *spriteManager);
    scoreMenu = std::make_unique<MenuLayout<3>>(
            rendererSize,
            750,
            renderClock,
            std::array<const Sprite *, 3>{
                    &spriteManager->get("all_time"),
                    &spriteManager->get("today"),
                    &spriteManager->get("exit")});
    myScoresMenu = std::make_unique<MenuLayout<3>>(
            rendererSize,
            750,
            renderClock,
            std::array<const Sprite *, 3>{
                    &spriteManager->get("top_scores"),
                    &spriteManager->get("latest_scores"),
                    &spriteManager->get("exit")});
    topScoreBoard = std::make_unique<ScrollingScoreBoard>(rendererSize, scrollPxPerMs, *spriteManager, renderClock);
    levelOverlay = makeOverlay("level");
    gameOverOverlay = makeOverlay("gameover");
    levelsCompletedOverlay = makeOverlay("levels_completed");
    trainingCompletedOverlay = makeOverlay("training_completed");
    replayExitOverlay = std::make_unique<ExitOverlay>(rendererSize, configuration.meterMargin,
                                                      spriteManager->get("exit"), renderClock, 750);
    trainingExitOverlay = std::make_unique<ExitOverlay>(rendererSize, configuration.meterMargin,
                                                        spriteManager->get("tap_flags_exit"), renderClock, 750);
    playingExitOverlay = std::make_unique<ExitOverlay>(rendererSize, configuration.meterMargin,
                                                       spriteManager->get("tap_score_exit"), renderClock, 750, 5'000);
    speedUpOverlay = std::make_unique<SpeedUpOverlay>(rendererSize, configuration.meterMargin, *spriteManager,
                                                      renderClock);
}

void trippin::Game::initClock() {
    renderClock.init();
    timer = std::make_unique<Timer>([this](int fps) {
        fpsStats.add(fps);
        ticksPerFrame.clear();
    });
}

std::unique_ptr<trippin::Level> trippin::Game::nextLevel() {
    auto lvl = std::make_unique<Level>();
    lvl->setWindowSize(rendererSize);
    lvl->setConfiguration(&configuration);
    lvl->setScale(scale.get());
    lvl->setSpriteManager(spriteManager.get());
    lvl->setSoundManager(&soundManager);
    lvl->setRenderClock(renderClock);
    lvl->setScore(score);
    lvl->setExtraLives(extraLives);
    lvl->setLastLevel(false);
    if (state == State::TITLE || state == State::START_MENU) {
        lvl->setMapName(configuration.loadMap);
        lvl->setAutoPlay(autoPlay.events);
    } else if (state == State::TRAINING) {
        lvl->setMapName(configuration.trainMaps[trainingStage]);
        lvl->setTraining(trainingStage, trainingProgress);
    } else if (state == State::REPLAY) {
        replayAutoPlay = convertEvents(replayScore.events[replayOffset]); // ensure extended lifetime for replay vec
        lvl->setAutoPlay(replayAutoPlay);
        lvl->setMapName(configuration.maps[levelIndex]);
        lvl->setLastLevel(levelIndex == configuration.maps.size() - 1);
        transferSurfaces();
    } else {
        lvl->setMapName(configuration.maps[levelIndex]);
        lvl->setLastLevel(levelIndex == configuration.maps.size() - 1);
        transferSurfaces();
    }
    lvl->init();
    return lvl;
}

void trippin::Game::advanceLevel() {
    lastTicks = -1; // represents no-last-ticks
    level->stop();
    level = nextLevel();
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
    UserInput ui(rendererSize, sdlSystem->getHighDpiScale());
    while (state != State::EXIT) {
        auto event = ui.pollEvent();
        if (!event) { // events found in event queue, but none relevant, poll again
            continue;
        }

        if (event.quit) {
            level->stop();
            break;
        }

        auto gogginInput = event.asGogginInput();
        if (gogginInput) {
            level->onInput(gogginInput, state == State::REPLAY);
        }

        if (!renderClock.isPaused() && (event.pKeyDown || event.focusLost)) {
            logger->log("paused, focusLost=" + std::to_string(event.focusLost));
            renderClock.pause();
            level->pause();
        }

        if (renderClock.isPaused() && (event.rKeyDown || event.focusGained)) {
            logger->log("resumed, focusGained=" + std::to_string(event.focusGained));
            renderClock.resume();
            level->resume();
        }

        if (renderClock.isPaused()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // avoid spinning in a tight loop
        } else {
            handle(event);
        }

        if (event.render) {
            render();
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
    logger->log(std::string("op=state_change, prev=") + prev + ", next=" + next);
}

void trippin::Game::render() {
    renderClock.update();

    SDL_SetRenderDrawColor(sdlSystem->getRenderer(), 244, 251, 255, 255);
    SDL_RenderClear(sdlSystem->getRenderer());

    level->render();

    if (state == State::TITLE) {
        titleOverlay->render();
    } else if (state == State::START_MENU) {
        titleMenu->render();
    } else if (state == State::SCORE_MENU) {
        scoreMenu->render();
    } else if (state == State::MY_SCORES_MENU) {
        myScoresMenu->render();
    } else if (state == State::ALL_TIME_SCORES) {
        topScoreBoard->render();
    } else if (state == State::TODAY_SCORES) {
        topScoreBoard->render();
    } else if (state == State::MY_TOP_SCORES) {
        topScoreBoard->render();
    } else if (state == State::MY_LATEST_SCORES) {
        topScoreBoard->render();
    } else if (state == State::LEVEL_TRANSITION) {
        levelOverlay->render();
    } else if (state == State::GAME_OVER) {
        gameOverOverlay->render();
    } else if (state == State::LEVELS_COMPLETED) {
        levelsCompletedOverlay->render();
    } else if (state == State::END_MENU) {
        endMenu->render();
    } else if (state == State::NAME_FORM) {
        nameForm->render();
    } else if (state == State::REPLAY) {
        replayExitOverlay->render();
        speedUpOverlay->render();
    } else if (state == State::TRAINING) {
        trainingExitOverlay->render();
    } else if (state == State::TRAINING_COMPLETED) {
        trainingExitOverlay->render();
        trainingCompletedOverlay->render();
    } else if (state == State::PLAYING) {
        playingExitOverlay->render();
    }

    SDL_RenderPresent(sdlSystem->getRenderer());

    auto ticks = level->getTicks();
    if (lastTicks != -1) {
        auto diff = ticks - lastTicks;
        ticksPerFrame[diff]++;
    }
    lastTicks = ticks;

    timer->next();
}

void trippin::Game::handle(UserInput::Event &event) {
    if (state == State::TITLE) {
        if (!titleOverlay->hasScores() && stagingArea->bothSet()) {
            titleOverlay->setScores(stagingArea->getTodayScores(15), stagingArea->getTopScores(15));
        }
        if (event.anythingPressed()) {
            titleMenu->reset();
            state = State::START_MENU;
            logStateChange("TITLE", "START_MENU");
        }
    } else if (state == State::START_MENU) {
        if (titleMenu->contains(0, event.touchPoint)) {
            extraLives = configuration.extraLives;
            levelIndex = 0;
            score = 0;
            gameId++;
            inputEvents.clear();
            state = State::PLAYING;
            playingExitOverlay->reset();
            logger->log(std::string("op=state_change")
                        + ", prev=START_MENU"
                        + ", next=PLAYING"
                        + ", id=" + std::to_string(gameId)
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        } else if (titleMenu->contains(1, event.touchPoint)) {
            score = 0;
            trainingStage = 0;
            trainingProgress = 0;
            state = State::TRAINING;
            trainingExitOverlay->reset();
            logger->log(std::string("op=state_change")
                        + ", prev=START_MENU"
                        + ", next=TRAINING"
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        } else if (titleMenu->contains(2, event.touchPoint)) {
            state = State::SCORE_MENU;
            scoreMenu->reset();
            logStateChange("START_MENU", "SCORE_MENU");
        } else if (titleMenu->contains(3, event.touchPoint)) {
            state = State::MY_SCORES_MENU;
            myScoresMenu->reset();
            logStateChange("START_MENU", "MY_SCORES_MENU");
        }
    } else if (state == State::SCORE_MENU) {
        if (scoreMenu->contains(2, event.touchPoint)) {
            titleMenu->reset();
            state = State::START_MENU;
            logStateChange("SCORE_MENU", "START_MENU");
        } else if (scoreMenu->contains(0, event.touchPoint)) {
            state = State::ALL_TIME_SCORES;
            topScoreBoard->reset();
            topScoreBoard->setScores(stagingArea->getTopScores(25));
            logStateChange("SCORE_MENU", "ALL_TIME_SCORES");
        } else if (scoreMenu->contains(1, event.touchPoint)) {
            state = State::TODAY_SCORES;
            topScoreBoard->reset();
            topScoreBoard->setScores(stagingArea->getTodayScores(25));
            logStateChange("SCORE_MENU", "TODAY_SCORES");
        }
    } else if (state == State::MY_SCORES_MENU) {
        if (myScoresMenu->contains(2, event.touchPoint)) {
            titleMenu->reset();
            state = State::START_MENU;
            logStateChange("MY_SCORES_MENU", "START_MENU");
        } else if (myScoresMenu->contains(0, event.touchPoint)) {
            state = State::MY_TOP_SCORES;
            topScoreBoard->reset();
            topScoreBoard->setScores(myScores->getTopScores());
            logStateChange("MY_SCORES_MENU", "MY_TOP_SCORES");
        } else if (myScoresMenu->contains(1, event.touchPoint)) {
            state = State::MY_LATEST_SCORES;
            topScoreBoard->reset();
            topScoreBoard->setScores(myScores->getLatestScores());
            logStateChange("MY_SCORES_MENU", "MY_LATEST_SCORES");
        }
    } else if (state == State::ALL_TIME_SCORES
               || state == State::TODAY_SCORES
               || state == State::MY_TOP_SCORES
               || state == State::MY_LATEST_SCORES) {
        const char *prevLabel;
        if (state == State::ALL_TIME_SCORES) {
            prevLabel = "ALL_TIME_SCORES";
        } else if (state == State::TODAY_SCORES) {
            prevLabel = "TODAY_SCORES";
        } else if (state == State::MY_TOP_SCORES) {
            prevLabel = "MY_TOP_SCORES";
        } else if (state == State::MY_LATEST_SCORES) {
            prevLabel = "MY_LATEST_SCORES";
        }
        if (event.anythingPressed()) {
            auto scoreClicked = topScoreBoard->onClick(event.touchPoint);
            if (scoreClicked) {
                replayScore = *scoreClicked;
                replayOffset = 0;
                extraLives = configuration.extraLives;
                levelIndex = 0;
                score = 0;
                state = State::REPLAY;
                replayExitOverlay->reset();
                speedUpOverlay->reset();
                logger->log(std::string("op=state_change")
                            + ", prev=" + prevLabel
                            + ", next=REPLAY"
                            + ", id=" + replayScore.id
                            + ", game=" + std::to_string(replayScore.game)
                            + ", name=" + replayScore.name
                            + ", tps=" + formatTps()
                            + ", fps=" + formatFps());
                advanceLevel();
            } else {
                titleMenu->reset();
                state = State::START_MENU;
                logStateChange(prevLabel, "START_MENU");
            }
        }
    } else if (state == State::REPLAY) {
        auto endFn = [this] {
            titleMenu->reset();
            state = State::START_MENU;
            level->onInput({false, true, false, false}, true);
            logger->log(std::string("op=state_change")
                        + ", prev=REPLAY"
                        + ", next=START_MENU"
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
        };
        if (event.anythingPressed()) {
            if (replayExitOverlay->exitClicked(event.touchPoint)) {
                endFn();
            }
        }
        if (level->ended()) {
            score = level->getScore(); // capture score to carry into next level
            if (level->completed()) {
                if (levelIndex < configuration.maps.size() - 1 && replayOffset < replayScore.events.size() - 1) {
                    replayOffset++; // advance replay events on level change
                    levelIndex++;
                    logger->log(std::string("op=replay_level")
                                + ", replayEvents=" + std::to_string(replayOffset)
                                + ", level=" + std::to_string(levelIndex)
                                + ", tps=" + formatTps()
                                + ", fps=" + formatFps());
                    advanceLevel();
                } else {
                    endFn();
                }
            } else {
                if (extraLives > 0) {
                    replayOffset++; // advance replay events on level restart
                    extraLives--;
                    logger->log(std::string("op=replay_extra_life")
                                + ", replayEvents=" + std::to_string(replayOffset)
                                + ", extraLives=" + std::to_string(extraLives)
                                + ", tps=" + formatTps()
                                + ", fps=" + formatFps());
                    advanceLevel();
                } else {
                    endFn();
                }
            }
        }
    } else if (state == State::PLAYING) {
        if (event.anythingPressed() && scoreArea.contains(event.touchPoint)) {
            titleMenu->reset();
            state = State::START_MENU;
            logStateChange("PLAYING", "START_MENU");
        } else if (level->ended()) {
            score = level->getScore();
            inputEvents.push_back(level->takeInputEvents());
            if (level->completed()) {
                if (levelIndex < configuration.maps.size() - 1) {
                    state = State::LEVEL_TRANSITION;
                    levelOverlay->reset(static_cast<int>(levelIndex));
                    logger->log(std::string("op=state_change")
                                + ", prev=PLAYING"
                                + ", next=LEVEL_TRANSITION"
                                + ", level=" + std::to_string(levelIndex)
                                + ", events=" + std::to_string(inputEvents.rbegin()->size())
                                + ", ticks=" + std::to_string(level->getTicks())
                                + ", score=" + std::to_string(score)
                                + ", tps=" + formatTps()
                                + ", fps=" + formatFps());
                } else {
                    state = State::LEVELS_COMPLETED;
                    levelsCompletedOverlay->reset();
                    logger->log(std::string("op=state_change")
                                + ", prev=PLAYING"
                                + ", next=LEVELS_COMPLETED"
                                + ", level=" + std::to_string(levelIndex)
                                + ", events=" + std::to_string(inputEvents.rbegin()->size())
                                + ", ticks=" + std::to_string(level->getTicks())
                                + ", score=" + std::to_string(score)
                                + ", tps=" + formatTps()
                                + ", fps=" + formatFps());
                }
            } else {
                if (extraLives > 0) {
                    extraLives--;
                    state = State::EXTRA_LIFE_DELAY;
                    auto now = std::chrono::steady_clock::now();
                    extraLifeTime = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
                    logger->log(std::string("op=state_change")
                                + ", prev=PLAYING"
                                + ", next=EXTRA_LIFE_DELAY"
                                + ", level=" + std::to_string(levelIndex)
                                + ", events=" + std::to_string(inputEvents.rbegin()->size())
                                + ", ticks=" + std::to_string(level->getTicks())
                                + ", score=" + std::to_string(score)
                                + ", extraLives=" + std::to_string(extraLives)
                                + ", tps=" + formatTps()
                                + ", fps=" + formatFps());
                } else {
                    state = State::GAME_OVER;
                    gameOverOverlay->reset();
                    logger->log(std::string("op=state_change")
                                + ", prev=PLAYING"
                                + ", next=GAME_OVER"
                                + ", level=" + std::to_string(levelIndex)
                                + ", events=" + std::to_string(inputEvents.rbegin()->size())
                                + ", ticks=" + std::to_string(level->getTicks())
                                + ", score=" + std::to_string(score)
                                + ", tps=" + formatTps()
                                + ", fps=" + formatFps());
                }
            }
        }
    } else if (state == State::GAME_OVER) {
        if (event.anythingPressed()) {
            state = State::END_MENU;
            endMenu->reset();
            logStateChange("GAME_OVER", "END_MENU");
        }
    } else if (state == State::LEVELS_COMPLETED) {
        if (event.anythingPressed()) {
            state = State::END_MENU;
            endMenu->reset();
            logStateChange("LEVELS_COMPLETED", "END_MENU");
        }
    } else if (state == State::EXTRA_LIFE_DELAY) {
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
        if (now - extraLifeTime >= std::chrono::seconds{1}) {
            state = State::PLAYING;
            logger->log(std::string("op=state_change")
                        + ", prev=EXTRA_LIFE_DELAY"
                        + ", next=PLAYING"
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        }
    } else if (state == State::TRAINING) {
        if (event.anythingPressed() && flagsArea.contains(event.touchPoint)) {
            score = 0;
            titleMenu->reset();
            state = State::START_MENU;
            logger->log(std::string("op=state_change")
                        + ", prev=TRAINING"
                        + ", next=START_MENU"
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        } else if (level->trainingCompleted()) {
            if (trainingStage == configuration.trainMaps.size() - 1) { // last training stage
                state = State::TRAINING_COMPLETED;
                trainingExitOverlay->reset();
                trainingCompletedOverlay->reset();
                logger->log(std::string("op=state_change")
                            + ", prev=TRAINING"
                            + ", next=TRAINING_COMPLETED"
                            + ", tps=" + formatTps()
                            + ", fps=" + formatFps());
            } else {
                trainingStage++;
                trainingProgress = 0;
                trainingExitOverlay->reset();
                logger->log(std::string("op=next_training_stage")
                            + ", trainingStage=" + std::to_string(trainingStage)
                            + ", tps=" + formatTps()
                            + ", fps=" + formatFps());
                advanceLevel();
            }
        } else if (level->ended()) {
            trainingExitOverlay->reset();
            trainingProgress = level->getTrainingProgress();
            logger->log(std::string("op=training_stage_repeat")
                        + ", trainingStage=" + std::to_string(trainingStage)
                        + ", trainingProgress=" + std::to_string(trainingProgress)
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        }
    } else if (state == State::TRAINING_COMPLETED) {
        if (event.anythingPressed() && flagsArea.contains(event.touchPoint)) {
            score = 0;
            titleMenu->reset();
            state = State::START_MENU;
            logger->log(std::string("op=state_change")
                        + ", prev=TRAINING_COMPLETED"
                        + ", next=START_MENU"
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        }
    } else if (state == State::LEVEL_TRANSITION) {
        if (event.anythingPressed()) {
            levelIndex++;
            state = State::PLAYING;
            logger->log(std::string("op=state_change")
                        + ", prev=LEVEL_TRANSITION"
                        + ", next=PLAYING"
                        + ", level=" + std::to_string(levelIndex)
                        + ", score=" + std::to_string(score)
                        + ", tps=" + formatTps()
                        + ", fps=" + formatFps());
            advanceLevel();
        }
    } else if (state == State::END_MENU) {
        if (endMenu->contains(1, event.touchPoint)) {
            titleMenu->reset();
            state = State::START_MENU;
            logStateChange("END_MENU", "START_MENU");
        } else if (endMenu->contains(0, event.touchPoint)) {
            state = State::NAME_FORM;
            nameForm->reset();
            logStateChange("END_MENU", "NAME_FORM");
        }
    } else {
        nameForm->onClick(event.touchPoint);
        if (nameForm->nameEntered()) {
            Score sc{appId, gameId, score, nameForm->getName(), convertInputEvents()};
            stagingArea->addScore(sc);
            scoreDb->add(sc);
            state = State::START_MENU;
            titleMenu->reset();
            auto sum = std::accumulate(inputEvents.begin(), inputEvents.end(), 0, [](std::size_t sum, const auto &vec) {
                return sum + vec.size();
            });
            logger->log(std::string("op=state_change")
                        + ", prev=NAME_FORM"
                        + ", next=START_MENU"
                        + ", name=" + nameForm->getName()
                        + ", score=" + std::to_string(score)
                        + ", events=" + std::to_string(sum));
        }
    }
}

std::vector<std::vector<trippin::Score::InputEvent>> trippin::Game::convertInputEvents() const {
    std::vector<std::vector<Score::InputEvent>> result;
    for (auto &vec: inputEvents) {
        std::vector<Score::InputEvent> v;
        v.reserve(vec.size());
        for (auto &e: vec) {
            v.push_back(Score::InputEvent::fromFlags(e.tick, e.jumpCharge, e.jumpRelease, e.duckStart, e.duckEnd));
        }
        result.push_back(v);
    }
    return result;
}

std::vector<trippin::GogginInputTick> trippin::Game::convertEvents(const std::vector<Score::InputEvent> &events) {
    std::vector<GogginInputTick> result;
    result.reserve(events.size());
    for (auto &e: events) {
        result.push_back({{e.jumpCharge(), e.jumpRelease(), e.duckStart(), e.duckEnd()}, e.tick});
    }
    return result;
}

std::string trippin::Game::formatFps() const {
    return formatTps(fpsStats.getMin(), fpsStats.getMax(), fpsStats.getAvg());
}

std::string trippin::Game::formatTps() const {
    return formatTps(level->getMinTps(), level->getMaxTps(), level->getAvgTps());
}

std::string trippin::Game::formatTps(int min, int max, int avg) {
    return std::to_string(min) + "/" + std::to_string(max) + "/" + std::to_string(avg);
}
