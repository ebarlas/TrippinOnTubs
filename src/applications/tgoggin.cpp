#include <sstream>
#include <iomanip>
#include "SDL_thread.h"
#include "sprite/SpriteManager.h"
#include "engine/Engine.h"
#include "sprite/Camera.h"
#include "gameloop.h"

std::string format(double d, int precision = 2) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << (d >= 0 ? "" : "_") << std::abs(d);
    return stream.str();
}

class SpriteObject : public trippin::Object {
public:
    const trippin::Sprite *sprite{};

    void init() {
        auto hb = sprite->getHitBox();
        size = {hb.w, hb.h};
        updateRounded();
    }

    virtual void render(const GameState &gs, const trippin::Camera &camera) {
        auto hb = sprite->getHitBox();
        auto &viewport = camera.getViewport();
        auto size = sprite->getSize();
        trippin::Rect<int> box{roundedPosition.x - hb.x, roundedPosition.y - hb.y, size.x, size.y};
        if (box.intersect(viewport)) {
            auto frame = (SDL_GetTicks() / sprite->getDuration()) % sprite->getFrames();
            sprite->render({box.x - viewport.x, box.y - viewport.y}, frame);
        }
    }
};

class Ball : public SpriteObject {
public:
    trippin::Camera *camera{};
    trippin::Scale scale{};
    double gameTicksPerSecond{};
    double gameTicksPerSecondSq{};

    void init() {
        auto mul = scaleMultiplier(scale);
        auto pixelsPerMeter = 240 * mul;
        auto yAccel = (8.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        auto xTerminal = (10.0 * pixelsPerMeter) / gameTicksPerSecond;
        auto yTerminal = (35.0 * pixelsPerMeter) / gameTicksPerSecond;
        auto xFriction = (1.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        
        platform = false;
        position.x = 200 + (std::rand() % camera->getUniverse().w / 2);
        position.y = (4880 * mul) - (600 + std::rand() % 100) * mul;
        acceleration = {0, yAccel};
        friction = {xFriction, 0};
        terminalVelocity = {xTerminal, yTerminal};
        platformCollisionType.set(trippin::PlatformCollisionType::reflective);
        mass = sprite->getHitBox().area();

        SpriteObject::init();
    }
};

class Goggin : public SpriteObject {
public:
    double runAccel{};
    double gravAccel{};
    double fallGravAccel{};
    double jumpVel{};
    trippin::Scale scale{};
    double gameTicksPerSecond{};
    double gameTicksPerSecondSq{};

    enum State {
        running,
        launching,
        rising,
        falling
    };

    State state = State::falling;
    Uint32 firstFrame{};

    void init() {
        auto pixelsPerMeter = 240 * scaleMultiplier(scale);
        runAccel = (7.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        gravAccel = (10.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        fallGravAccel = (18.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        jumpVel = (-8.5 * pixelsPerMeter) / gameTicksPerSecond;
        auto xTerminal = (8.0 * pixelsPerMeter) / gameTicksPerSecond;
        auto yTerminal = (18.0 * pixelsPerMeter) / gameTicksPerSecond;

        auto hb = sprite->getHitBox();
        platform = false;
        position = {static_cast<double>(hb.corner().x), static_cast<double>(hb.corner().y)};
        acceleration = {0, 0};
        gravity = {0, gravAccel};
        fallGravity = fallGravAccel;
        terminalVelocity = {xTerminal, yTerminal};
        mass = hb.area();

        SpriteObject::init();

        firstFrame = SDL_GetTicks() / sprite->getDuration();
    }

    void render(const GameState &gs, const trippin::Camera &camera) override {
        auto hb = sprite->getHitBox();
        auto &viewport = camera.getViewport();
        auto size = sprite->getSize();
        trippin::Rect<int> box{roundedPosition.x - hb.x, roundedPosition.y - hb.y, size.x, size.y};
        if (box.intersect(viewport)) {
            auto frame = SDL_GetTicks() / sprite->getDuration();
            if (state == running) {
                auto runFrame = (frame - firstFrame) % 8;
                sprite->render({box.x - viewport.x, box.y - viewport.y}, runFrame);
            } else if (state == launching || state == rising) {
                auto riseFrame = 8 + std::min(3, static_cast<int>(frame - firstFrame));
                sprite->render({box.x - viewport.x, box.y - viewport.y}, riseFrame);
            } else {
                auto fallFrame = 12 + std::min(2, static_cast<int>(frame - firstFrame));
                sprite->render({box.x - viewport.x, box.y - viewport.y}, fallFrame);
            }
        }
    }

    void afterTick() override {
        if (platformCollisions.testBottom()) {
            acceleration = {runAccel, 0};
        } else {
            acceleration = {0, 0};
        }

        auto frame = SDL_GetTicks() / sprite->getDuration();

        if (state == State::running && platformCollisions.testBottom() && std::rand() % 200 == 0) {
            state = State::launching;
            firstFrame = frame;
        } else if(state == State::launching && frame - firstFrame > 2) {
            state = State::rising;
            velocity.y = jumpVel;
        } else if (state == rising && velocity.y >= 0) {
            state = State::falling;
            firstFrame = frame;
        } else if (state == falling && platformCollisions.testBottom()) {
            state = State::running;
            firstFrame = frame;
        }
    }
};

class Game {
public:
    trippin::Engine engine{};
    std::unique_ptr<trippin::SpriteManager> spriteManager{};
    Goggin goggin{};
    std::vector<SpriteObject *> grounds{};
    std::vector<Ball *> balls{};
    trippin::Camera camera{};

    void create() {
        auto initFn = [this](const GameState &gs) {
            init(gs);
        };

        auto updateFn = [this](const GameState &gs) {

        };

        auto renderFn = [this](const GameState &gs) {
            render(gs);
        };

        gameLoop(initFn, updateFn, renderFn);
    }

    void init(const GameState &gs) {
        auto scale = trippin::Scale::xxsmall;
        auto mul = scaleMultiplier(scale);

        spriteManager = std::make_unique<trippin::SpriteManager>(gs.renderer, scale);
        auto &groundSprite = spriteManager->get(trippin::SpriteType::ground);
        auto &ballSprite = spriteManager->get(trippin::SpriteType::ball);
        auto &gogginSprite = spriteManager->get(trippin::SpriteType::goggin);

        int numGroundPlatforms = 20;
        int numBalls = 20;
        int universeHeight = 8'000;

        camera.setViewport({0, 0, gs.windowSize.x, gs.windowSize.y});
        camera.setUniverse({0, 0,
                            numGroundPlatforms * groundSprite.getSize().x,
                            static_cast<int>(universeHeight * mul)});

        auto tickPeriod = 5;
        auto gameTicksPerSecond = 1000.0 / tickPeriod;
        auto gameTicksPerSecondSq = gameTicksPerSecond * gameTicksPerSecond;

        engine.setTickPeriod(tickPeriod);
        engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
        engine.setObjectCollisionType(trippin::ObjectCollisionType::inelastic);

        int nextId = 1;

        goggin.scale = scale;
        goggin.setId(nextId++);
        goggin.sprite = &gogginSprite;
        goggin.gameTicksPerSecond = gameTicksPerSecond;
        goggin.gameTicksPerSecondSq = gameTicksPerSecondSq;
        goggin.init();
        engine.add(&goggin);

        for (int i = 0; i < numGroundPlatforms; i++) {
            auto ground = new SpriteObject;
            ground->setId(nextId++);
            ground->setPlatform(true);
            ground->setPosition(trippin::Point<double>{i * 480.0, 4880} * mul);
            ground->sprite = &groundSprite;
            ground->init();
            grounds.push_back(ground);
            engine.add(ground);
        }

        for (int i = 0; i < numBalls; i++) {
            auto ball = new Ball;
            ball->gameTicksPerSecond = gameTicksPerSecond;
            ball->gameTicksPerSecondSq = gameTicksPerSecondSq;
            ball->scale = scale;
            ball->camera = &camera;
            ball->sprite = &ballSprite;
            ball->setId(nextId++);
            ball->init();
            balls.push_back(ball);
            engine.add(ball);
        }

        engine.start();
    }

    void render(const GameState &gs) {
        auto center = goggin.getRoundedCenter();
        camera.centerOn(center);
        goggin.render(gs, camera);
        for (auto ground : grounds) {
            ground->render(gs, camera);
        }
        for (auto ball : balls) {
            ball->render(gs, camera);
        }
    }
};

int main( int argc, char* args[] ) {
    Game game;
    game.create();
    return 0;
}