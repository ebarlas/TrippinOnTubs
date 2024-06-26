#include "Level.h"
#include "Ground.h"
#include "GameObject.h"
#include "WingedTub.h"
#include "DecorativeObject.h"
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

void trippin::Level::setTraining(unsigned int stage, int progress) {
    training = true;
    trainingStage = stage;
    trainingProgress = progress;
}

void trippin::Level::setLastLevel(bool b) {
    lastLevel = b;
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

    levelStats = std::make_unique<LevelStats>();

    scoreTicker = std::make_unique<ScoreTicker>(
            configuration->meterMargin,
            spriteManager->get("digits"),
            initialScore,
            Rect<int>{0, 0, windowSize.x, windowSize.y},
            sceneBuilder);

    auto &comboSprite = spriteManager->get("combo");
    auto notificationsTop = windowSize.y / 4;

    notificationDrawer = std::make_unique<NotificationDrawer>(
            configuration->msPerTick(),
            windowSize.x,
            notificationsTop,
            configuration->meterMargin,
            configuration->meterMargin / 2,
            sceneBuilder);

    comboNotificationManager = std::make_unique<NotificationManager>(
            comboSprite,
            spriteManager->get("digits"),
            *notificationDrawer);

    comboManager = std::make_unique<ComboManager>(*comboNotificationManager, configuration->minComboHits);

    groupNotificationManager = std::make_unique<NotificationManager>(
            spriteManager->get("group_bonus"),
            spriteManager->get("digits"),
            *notificationDrawer);

    // define goggin object prior to other game objects
    for (auto &obj: map.objects) {
        if (obj.type == "goggin") {
            auto gogginHb = spriteManager->get("goggin").getEngineHitBox();
            pointCloudManager = std::make_unique<PointCloudManager>(
                    Point<int>{gogginHb.w * 2, gogginHb.h},
                    Point<int>{gogginHb.w * 6, gogginHb.h * 2},
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
                    sceneBuilder,
                    *levelStats,
                    *scoreTicker);
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
        Activation activation{
                obj.activation > 0 ? obj.activation : configuration->activationProximity,
                configuration->deactivationProximity,
                *goggin,
                {0, 0, map.universe.x, map.universe.y}};
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
                    *configuration,
                    obj,
                    spriteManager->get(obj.type),
                    activation,
                    *goggin,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder,
                    *levelStats);
            engine.addListener(wingedTub.get());
            objects.push_back(std::move(wingedTub));
        } else if (obj.type == "double_jump_arc"
                   || obj.type == "jump_arc"
                   || obj.type == "charged_jump_arc"
                   || obj.type == "duck_line"
                   || obj.type == "jump_slam_down_arc"
                   || obj.type == "charged_duck_jump_arc") {
            auto decObj = std::make_unique<DecorativeObject>(
                    obj,
                    spriteManager->get(obj.type),
                    activation,
                    *camera,
                    sceneBuilder);
            engine.addListener(decObj.get());
            objects.push_back(std::move(decObj));
        } else if (obj.type == "running_clock") {
            auto runningClock = std::make_unique<RunningClock>(
                    *configuration,
                    *configuration->findObject(obj.type),
                    obj,
                    spriteManager->get(obj.type),
                    spriteManager->get("time_bonus"),
                    *goggin,
                    spirit,
                    activation,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder,
                    *notificationDrawer);
            engine.add(runningClock.get());
            engine.addListener(runningClock.get());
            objects.push_back(std::move(runningClock));
        } else if (obj.type == "zombie"
                   || obj.type == "zombie_green"
                   || obj.type == "rat"
                   || obj.type == "bird"
                   || obj.type == "ball"
                   || obj.type == "fireball"
                   || obj.type == "brick"
                   || obj.type == "chicken") {
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
                    sceneBuilder,
                    groupManager,
                    *groupNotificationManager);
            engine.add(gameObject.get());
            engine.addListener(gameObject.get());
            objects.push_back(std::move(gameObject));
            if (obj.group) {
                groupManager.add(obj.group, obj.id);
            }
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
    spirit.delay(training ? 10 : configuration->spiritSecondsBehind); // init spirit way back for training

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

    // add separate goggin rendered last, to ensure goggin is laid over everything else
    // this must be separate from main goggin object, which much update the camera first (not last)
    engine.addListener(gogginRenderer.get());
    engine.add(goggin.get());

    if (!training) {
        engine.addListener(pointCloudManager.get());
        engine.addListener(scoreTicker.get());
    }

    engine.addListener(notificationDrawer.get());

    if (!training) {
        levelBonuses = std::make_unique<LevelBonuses>(
                *configuration,
                *goggin,
                *spiritClock,
                *notificationDrawer,
                *scoreTicker,
                *pointCloudManager,
                *camera,
                *soundManager,
                *spriteManager,
                lastLevel,
                initialExtraLives);
        engine.addListener(levelBonuses.get());
    }

    levelEndListener = std::make_unique<LevelEndListener>(*goggin, *scoreTicker);
    engine.addListener(levelEndListener.get());

    if (training) {
        trainingProgram = std::make_unique<TrainingProgram>(
                windowSize,
                configuration->meterMargin,
                *configuration,
                *spriteManager,
                *soundManager,
                *levelStats,
                *renderClock,
                sceneBuilder,
                trainingStage,
                trainingProgress);
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
    return levelEndListener->hasEnded();
}

bool trippin::Level::completed() {
    return goggin->rightOfUniverse();
}

bool trippin::Level::trainingCompleted() const {
    return trainingProgram->completed();
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

int trippin::Level::getMinTps() const {
    return engine.getMinTps();
}

int trippin::Level::getMaxTps() const {
    return engine.getMaxTps();
}

int trippin::Level::getAvgTps() const {
    return engine.getAvgTps();
}

int trippin::Level::getTrainingProgress() const {
    return trainingProgram->getProgress();
}
