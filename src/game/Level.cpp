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

void trippin::Level::setScale(const Scale *sc) {
    scale = sc;
}

void trippin::Level::setSpriteManager(trippin::SpriteManager *sm) {
    spriteManager = sm;
}

void trippin::Level::setSoundManager(trippin::SoundManager *sm) {
    soundManager = sm;
}

void trippin::Level::setRenderClock(const RenderClock &clock) {
    renderClock = &clock;
}

void trippin::Level::setAutoPlay(const std::vector<GogginInputTick> &ap) {
    autoPlay = &ap;
}

void trippin::Level::setMapName(const std::string &name) {
    mapName = &name;
}

void trippin::Level::setScore(int score) {
    initialScore = score;
}

void trippin::Level::setTraining(bool b) {
    training = b;
}

void trippin::Level::initMap() {
    map.load(*mapName);
    map.rescale(scale->getEngineFactor());
}

void trippin::Level::initCamera() {
    camera = std::make_unique<Camera>(
            Rect<int>{0, 0, map.universe.x, map.universe.y},
            Rect<int>{0, 0, windowSize.x, windowSize.y} * scale->getDeviceEngineFactor(),
            configuration->shakeAmplitude);
}

void trippin::Level::initEngine() {
    engine.setTickRate(configuration->tickRate);
    engine.setPlatformCollision(onAbsorbentCollision);
    engine.setObjectCollision(onElasticCollision1D);

    scoreTicker = std::make_unique<ScoreTicker>(
            configuration->meterMargin,
            spriteManager->get("digits"),
            initialScore,
            Rect<int>{0, 0, windowSize.x, windowSize.y},
            sceneBuilder);

    comboManager = std::make_unique<ComboManager>(
            *spriteManager,
            *scoreTicker,
            configuration->msPerTick(),
            windowSize,
            configuration->meterMargin,
            sceneBuilder);

    // define goggin object prior to other game objects
    for (auto &obj: map.objects) {
        if (obj.type == "goggin") {
            auto gogginHb = spriteManager->get("goggin").getEngineHitBox();
            auto gogginSize = Point<int>{gogginHb.w, gogginHb.h};
            pointCloudManager = std::make_unique<PointCloudManager>(
                    gogginSize * 2,
                    gogginSize * 6,
                    configuration->ticksPerSecond() * 2,
                    spriteManager->get("digits"),
                    sceneBuilder,
                    *camera);
            goggin = std::make_unique<Goggin>(
                    *configuration,
                    *configuration->findObject(obj.type),
                    obj,
                    *spriteManager,
                    *comboManager,
                    *pointCloudManager,
                    autoPlay,
                    map.universe,
                    *soundManager,
                    *camera,
                    sceneBuilder);
            gogginRenderer = std::make_unique<GogginRenderer>(*goggin);
            break;
        }
    }

    // add goggin first since centered camera is required for proper positioning
    // in particular, the first frame of the level will be badly skewed with no camera centering
    engine.addListener(goggin.get());

    if (!training) {
        engine.addListener(&spirit);
    }

    for (auto &layer: map.layers) {
        auto uptr = std::make_unique<Layer>(
                *configuration,
                *spriteManager,
                layer,
                *camera,
                sceneBuilder);
        engine.addListener(uptr.get());
        objects.push_back(std::move(uptr));
    }

    for (auto &obj: map.objects) {
        if (obj.type.rfind("ground_melt_", 0) == 0 || obj.type.rfind("platform", 0) == 0) {
            auto ground = std::make_unique<Ground>(
                    obj,
                    spriteManager->get(obj.type),
                    activation,
                    spirit,
                    *camera,
                    sceneBuilder);
            engine.add(ground.get());
            engine.addListener(ground.get());
            objects.push_back(std::move(ground));
        } else if (obj.type == "winged_tub") {
            auto wingedTub = std::make_unique<WingedTub>(
                    obj,
                    spriteManager->get(obj.type),
                    activation,
                    *goggin,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder);
            engine.addListener(wingedTub.get());
            objects.push_back(std::move(wingedTub));
        } else if (obj.type == "running_clock") {
            auto runningClock = std::make_unique<RunningClock>(
                    *configuration->findObject(obj.type),
                    obj,
                    spriteManager->get(obj.type),
                    *goggin,
                    spirit,
                    activation,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder);
            engine.add(runningClock.get());
            engine.addListener(runningClock.get());
            objects.push_back(std::move(runningClock));
        } else if (obj.type == "zombie" || obj.type == "rat" || obj.type == "bird" || obj.type == "ball" ||
                   obj.type == "brick") {
            auto gameObject = std::make_unique<GameObject>(
                    *configuration,
                    *configuration->findObject(obj.type),
                    obj,
                    spriteManager->get(obj.type),
                    *goggin,
                    activation,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder);
            engine.add(gameObject.get());
            engine.addListener(gameObject.get());
            objects.push_back(std::move(gameObject));
        }
    }

    lifeMeter = std::make_unique<LifeMeter>(
            configuration->meterMargin,
            spriteManager->get("goggin_head"),
            initialExtraLives,
            Rect<int>{0, 0, windowSize.x, windowSize.y},
            sceneBuilder);
    if (!training) {
        engine.addListener(lifeMeter.get());
    }

    spirit.setTicksPerSecond(configuration->ticksPerSecond());
    spirit.setVelocity(goggin->terminalVelocity.x);
    spirit.delay(configuration->spiritSecondsBehind);

    jumpMeter = std::make_unique<JumpMeter>(
            spriteManager->get("winged_foot"),
            *goggin,
            Point<int>{configuration->meterMargin, configuration->meterMargin},
            sceneBuilder);
    engine.addListener(jumpMeter.get());

    spiritClock = std::make_unique<SpiritClock>(
            *configuration,
            spriteManager->get("clock_timer"),
            spirit,
            *goggin,
            Point<int>{configuration->meterMargin,
                       configuration->meterMargin + spriteManager->get("winged_foot").getDeviceSize().y},
            spriteManager->get("ground_melt_left").getDeviceSize().x * scale->getDeviceEngineFactor(),
            sceneBuilder);
    if (!training) {
        engine.addListener(spiritClock.get());
    }

    activation.setUniverse(map.universe);
    activation.setActivationProximity(configuration->activationProximity);
    activation.setDeactivationProximity(configuration->deactivationProximity);
    activation.setGoggin(goggin.get());

    // add separate goggin rendered last, to ensure goggin is laid over everything else
    // this must be separate from main goggin object, which much update the camera first (not last)
    engine.addListener(gogginRenderer.get());
    engine.add(goggin.get());

    engine.addListener(pointCloudManager.get());

    if (!training) {
        engine.addListener(scoreTicker.get());
    }

    engine.addListener(comboManager.get());

    if (training) {
        trainingProgram = std::make_unique<TrainingProgram>(
                windowSize,
                configuration->meterMargin,
                *configuration,
                *spriteManager,
                *soundManager,
                *goggin,
                *renderClock,
                sceneBuilder);
        engine.addListener(trainingProgram.get());
    }

    engine.addListener(this);
}

void trippin::Level::init() {
    initMap();
    initCamera();
    initEngine();
}

void trippin::Level::onInput(GogginInput input, bool replay) {
    goggin->onUserInput(input);
    if (replay) {
        if (input.jumpCharge || input.duckStart) {
            engine.setTickRate(configuration->tickRate * configuration->fastReplayFactor);
        } else if (input.jumpRelease || input.duckEnd) {
            engine.setTickRate(configuration->tickRate);
        }
    }
}

void trippin::Level::render() {
    sceneBuilder.execute();
}

void trippin::Level::start() {
    engine.start();
    Mix_FadeInMusic(soundManager->getMusic(map.music), -1, 2'000);
}

bool trippin::Level::ended() {
    return goggin->belowUniverse() || goggin->rightOfUniverse();
}

bool trippin::Level::completed() {
    return training
           ? goggin->rightOfUniverse() || trainingProgram->completed()
           : goggin->rightOfUniverse();
}

void trippin::Level::stop() {
    engine.stop();
    engine.join();
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
}

int trippin::Level::getScore() const {
    return scoreTicker->getScore();
}

std::vector<trippin::GogginInputTick> trippin::Level::takeInputEvents() const {
    return goggin->takeInputEvents();
}

void trippin::Level::setExtraLives(int extraLives) {
    initialExtraLives = extraLives;
}

void trippin::Level::pause() {
    engine.pause();
    Mix_Pause(-1);
    Mix_PauseMusic();
}

void trippin::Level::resume() {
    engine.resume();
    Mix_Resume(-1);
    Mix_ResumeMusic();
}

void trippin::Level::beforeTick(int) {
    sceneBuilder.reset();
}

void trippin::Level::afterTick(int) {
    sceneBuilder.build();
}

int trippin::Level::getTicks() const {
    return engine.getTicks();
}