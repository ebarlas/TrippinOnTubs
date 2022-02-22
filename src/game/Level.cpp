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
    map.rescale(scale->multiplier);
    map.convert(configuration->tickRate);
}

void trippin::Level::initCamera() {
    camera = std::make_unique<Camera>(
            Rect<int>{0, 0, map.universe.x, map.universe.y},
            Rect<int>{0, 0, windowSize.x, windowSize.y},
            configuration->shakeAmplitude * scale->multiplier);
}

void trippin::Level::initEngine() {
    engine.setTicksPerFrame(ticksPerFrame);
    engine.setPlatformCollision(onAbsorbentCollision);
    engine.setObjectCollision(onElasticCollision1D);
    if (!training) {
        engine.addListener(&spirit);
    }

    int zIndex = 0;
    int zIndexTop = 100'000;

    scoreTicker = std::make_unique<ScoreTicker>(
            map.meterMargin,
            spriteManager->get("digits"),
            initialScore,
            camera->getViewport(),
            sceneBuilder,
            zIndexTop);
    if (!training) {
        engine.addListener(scoreTicker.get());
    }

    comboManager = std::make_unique<ComboManager>(
            *spriteManager,
            *scoreTicker,
            configuration->msPerTick(),
            windowSize,
            map.meterMargin,
            sceneBuilder,
            zIndexTop);
    engine.addListener(comboManager.get());

    for (auto &layer: map.layers) {
        auto uptr = std::make_unique<Layer>(
                *configuration,
                *spriteManager,
                layer,
                *camera,
                sceneBuilder,
                zIndex++);
        engine.addListener(uptr.get());
        objects.push_back(std::move(uptr));
    }

    // define goggin object prior to other game objects
    for (auto &obj: map.objects) {
        if (obj.type == "goggin") {
            auto &gogginSprite = spriteManager->get(obj.type);
            auto gogginHb = gogginSprite.getHitBox();
            auto gogginSize = Point<int>{gogginHb.w, gogginHb.h};
            pointCloudManager = std::make_unique<PointCloudManager>(
                    gogginSize * 2,
                    gogginSize * 6,
                    configuration->ticksPerSecond() * 2,
                    spriteManager->get("digits"),
                    sceneBuilder,
                    *camera,
                    zIndexTop);
            engine.addListener(pointCloudManager.get());

            goggin = std::make_unique<Goggin>(
                    *configuration,
                    obj,
                    *spriteManager,
                    *comboManager,
                    *pointCloudManager,
                    autoPlay,
                    map.universe,
                    *soundManager,
                    *camera,
                    sceneBuilder,
                    zIndexTop);
            engine.add(goggin.get());
            break;
        }
    }

    for (auto &obj: map.objects) {
        if (obj.type.rfind("ground_melt_", 0) == 0 || obj.type.rfind("platform", 0) == 0) {
            auto ground = std::make_unique<Ground>(
                    *configuration,
                    obj,
                    spriteManager->get(obj.type),
                    activation,
                    spirit,
                    *camera,
                    sceneBuilder,
                    zIndex);
            engine.add(ground.get());
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
                    zIndex);
            engine.addListener(wingedTub.get());
            objects.push_back(std::move(wingedTub));
        } else if (obj.type == "running_clock") {
            auto runningClock = std::make_unique<RunningClock>(
                    *configuration,
                    obj,
                    spriteManager->get(obj.type),
                    *goggin,
                    spirit,
                    activation,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder,
                    zIndex);
            engine.add(runningClock.get());
            objects.push_back(std::move(runningClock));
        } else if (obj.type == "zombie" || obj.type == "rat" || obj.type == "bird" || obj.type == "ball" ||
                   obj.type == "brick") {
            auto gameObject = std::make_unique<GameObject>(
                    *configuration,
                    obj,
                    spriteManager->get(obj.type),
                    *goggin,
                    activation,
                    *scoreTicker,
                    *soundManager,
                    *camera,
                    sceneBuilder,
                    zIndex);
            engine.add(gameObject.get());
            objects.push_back(std::move(gameObject));
        }
    }

    lifeMeter = std::make_unique<LifeMeter>(
            map.meterMargin,
            spriteManager->get("goggin_head"),
            initialExtraLives,
            camera->getViewport(),
            sceneBuilder,
            zIndexTop);
    if (!training) {
        engine.addListener(lifeMeter.get());
    }

    spirit.setTicksPerSecond(configuration->ticksPerSecond());
    spirit.setVelocity(goggin->terminalVelocity.x);
    spirit.delay(configuration->spiritSecondsBehind);

    jumpMeter = std::make_unique<JumpMeter>(
            spriteManager->get("winged_foot"),
            *goggin,
            Point<int>{map.meterMargin, map.meterMargin},
            sceneBuilder,
            zIndexTop);
    engine.addListener(jumpMeter.get());

    spiritClock = std::make_unique<SpiritClock>(
            *configuration,
            spriteManager->get("clock_timer"),
            spirit,
            *goggin,
            Point<int>{map.meterMargin, map.meterMargin + spriteManager->get("winged_foot").getSize().y},
            spriteManager->get("ground_melt_left").getSize().x,
            sceneBuilder,
            zIndexTop);
    if (!training) {
        engine.addListener(spiritClock.get());
    }

    activation.setUniverse(map.universe);
    activation.setActivationProximity(toInt(configuration->activationProximity * scale->multiplier));
    activation.setDeactivationProximity(toInt(configuration->deactivationProximity * scale->multiplier));
    activation.setGoggin(goggin.get());

    if (training) {
        trainingProgram = std::make_unique<TrainingProgram>(
                windowSize, map.meterMargin, *configuration, *spriteManager, *soundManager, *goggin, *renderClock,
                sceneBuilder, zIndexTop);
        engine.addListener(trainingProgram.get());
    }

    engine.addListener(this);
}

void trippin::Level::init() {
    initMap();
    initCamera();
    initEngine();
}

void trippin::Level::onInput(GogginInput input) {
    goggin->onUserInput(input);
}

void trippin::Level::render() {
    auto scene = sceneBuilder.take();

    // stable sort ensures that (1) zIndex order is observed and (2) dispatch order is observed for each zIndex
    std::stable_sort(scene.begin(), scene.end(), [](const auto &left, const auto &right) {
        return left.zIndex < right.zIndex;
    });

    for (auto &fn: scene) {
        fn.fn();
    }
}

void trippin::Level::update() {
    engine.onRender();
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

void trippin::Level::setExtraLives(int extraLives) {
    initialExtraLives = extraLives;
}

void trippin::Level::pause() {
    Mix_Pause(-1);
    Mix_PauseMusic();
}

void trippin::Level::resume() {
    Mix_Resume(-1);
    Mix_ResumeMusic();
}

void trippin::Level::beforeTick(Uint32 engineTicks) {
    sceneBuilder.reset();
}

void trippin::Level::afterTick(Uint32 engineTicks) {
    sceneBuilder.build();
}

int trippin::Level::getTicks() const {
    return engine.getTicks();
}

void trippin::Level::setTicksPerFrame(double tpf) {
    ticksPerFrame = tpf;
}
