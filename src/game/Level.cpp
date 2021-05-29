#include "Level.h"
#include "Ground.h"
#include "GameObject.h"
#include "WingedTub.h"
#include "RunningClock.h"
#include "Layer.h"
#include "engine/Convert.h"
#include "engine/Collisions.h"

void trippin::Level::setWindowSize(trippin::Point<int> ws) {
    windowSize = ws;
}

void trippin::Level::setConfiguration(Configuration *conf) {
    configuration = conf;
}

void trippin::Level::setScale(trippin::Configuration::Scale *sc) {
    scale = sc;
}

void trippin::Level::setSpriteManager(trippin::SpriteManager *sm) {
    spriteManager = sm;
}

void trippin::Level::setSoundManager(trippin::SoundManager *sm) {
    soundManager = sm;
}

void trippin::Level::setAutoPlay(const std::vector<GogginInputTick> &autoPlay) {
    goggin.setAutoPlay(autoPlay);
}

void trippin::Level::setMapName(const std::string &name) {
    mapName = &name;
}

void trippin::Level::setScore(int score) {
    scoreTicker.setScore(score);
}

void trippin::Level::initMap() {
    map.load(*mapName);
    map.rescale(scale->multiplier);
    map.convert(configuration->tickPeriod);
}

void trippin::Level::initCamera() {
    camera.setViewport({0, 0, windowSize.x, windowSize.y});
    camera.setUniverse({0, 0, map.universe.x, map.universe.y});
}

void trippin::Level::initEngine() {
    engine.setTickPeriod(configuration->tickPeriod);
    engine.setPlatformCollision(onAbsorbentCollision);
    engine.setObjectCollision(onElasticCollision1D);
    engine.addListener(&spirit);

    for (auto &layer : map.layers) {
        auto uptr = std::make_unique<Layer>();
        uptr->init(*spriteManager, layer);
        objects.push_back(std::move(uptr));
    }

    for (auto &obj : map.objects) {
        if (obj.type == "goggin") {
            goggin.setUniverse(map.universe);
            goggin.setDust(spriteManager->get("dust"));
            goggin.setDustBlast(spriteManager->get("dust_blast"));
            goggin.setWhiteDustBlast(spriteManager->get("dust_blast_white"));
            goggin.setDigits(spriteManager->get("digits"));
            goggin.setSoundManager(*soundManager);
            goggin.init(*configuration, obj, spriteManager->get(obj.type));
            engine.add(&goggin);
        } else if (obj.type.rfind("ground_melt_", 0) == 0 || obj.type.rfind("platform", 0) == 0) {
            auto uptr = std::make_unique<Ground>();
            uptr->init(*configuration, obj, spriteManager->get(obj.type));
            uptr->setSpirit(&spirit);
            uptr->setActivation(&activation);
            engine.add(uptr.get());
            objects.push_back(std::move(uptr));
        } else if (obj.type == "winged_tub") {
            auto uptr = std::make_unique<WingedTub>();
            uptr->setGoggin(&goggin);
            uptr->setScoreTicker(&scoreTicker);
            uptr->setActivation(&activation);
            uptr->setSoundManager(*soundManager);
            uptr->init(*configuration, obj, spriteManager->get(obj.type));
            engine.addListener(uptr.get());
            objects.push_back(std::move(uptr));
        } else if (obj.type == "running_clock") {
            auto uptr = std::make_unique<RunningClock>();
            uptr->setGoggin(&goggin);
            uptr->setSpirit(&spirit);
            uptr->setActivation(&activation);
            uptr->setScoreTicker(&scoreTicker);
            uptr->setSoundManager(*soundManager);
            uptr->init(*configuration, obj, spriteManager->get(obj.type));
            engine.add(uptr.get());
            objects.push_back(std::move(uptr));
        } else if (obj.type == "zombie" || obj.type == "rat" || obj.type == "bird" || obj.type == "ball" || obj.type == "brick") {
            auto uptr = std::make_unique<GameObject>();
            uptr->setActivation(&activation);
            uptr->setGoggin(goggin);
            uptr->setScoreTicker(scoreTicker);
            uptr->setSoundManager(*soundManager);
            uptr->init(*configuration, obj, spriteManager->get(obj.type));
            engine.add(uptr.get());
            objects.push_back(std::move(uptr));
        }
    }

    scoreTicker.setSprite(spriteManager->get("digits"));
    scoreTicker.setMargin(map.meterMargin);
    scoreTicker.setGoggin(&goggin);
    scoreTicker.init();

    spirit.setTicksPerSecond(configuration->ticksPerSecond());
    spirit.setVelocity(goggin.terminalVelocity.x);
    spirit.delay(configuration->spiritSecondsBehind);

    auto &timerSprite = spriteManager->get("clock_timer");
    spiritClock.setGoggin(goggin);
    spiritClock.setSpirit(spirit);
    spiritClock.setPosition({map.meterMargin, map.meterMargin});
    spiritClock.setPadding(spriteManager->get("ground_melt_left").getSize().x);
    spiritClock.init(*configuration, timerSprite);
    engine.addListener(&spiritClock);

    jumpMeter.setSprite(spriteManager->get("winged_foot"));
    jumpMeter.setGoggin(goggin);
    jumpMeter.setPosition({map.meterMargin, map.meterMargin + timerSprite.getSize().y});
    jumpMeter.init();
    engine.addListener(&jumpMeter);

    activation.setUniverse(map.universe);
    activation.setActivationProximity(toInt(configuration->activationProximity * scale->multiplier));
    activation.setDeactivationProximity(toInt(configuration->deactivationProximity * scale->multiplier));
    activation.setGoggin(&goggin);
}

void trippin::Level::init() {
    initMap();
    initCamera();
    initEngine();
}

void trippin::Level::render(GogginInput input) {
    goggin.onUserInput(input);
    goggin.centerCamera(camera);
    for (auto &obj : objects) {
        obj->render(camera);
    }

    goggin.render(camera);
    spiritClock.render(camera);
    jumpMeter.render(camera);
    scoreTicker.render(camera);
}

void trippin::Level::start() {
    engine.start();
    Mix_FadeInMusic(soundManager->getMusic(map.music), -1, 2'000);
}

bool trippin::Level::ended() {
    return !goggin.inUniverse();
}

bool trippin::Level::completed() {
    return goggin.rightOfUniverse();
}

void trippin::Level::stop() {
    engine.stop();
    engine.join();
    Mix_HaltMusic();
}

int trippin::Level::getScore() const {
    return scoreTicker.getScore();
}

void trippin::Level::pause() {
    engine.pause();
}

void trippin::Level::resume() {
    engine.resume();
}

