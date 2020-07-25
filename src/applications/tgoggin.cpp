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
    bool displayLabel{};
    SDL_Color fontColor{};

    void init(const trippin::Sprite *sp) {
        sprite = sp;
        auto &hb = sprite->getHitBox();
        size = {hb.w, hb.h};
        mass = hb.area();
        updateRounded();
    }

    void render(const GameState &gs, const trippin::Camera &camera) {
        auto &hb = sprite->getHitBox();
        auto &viewport = camera.getViewport();
        auto &size = sprite->getSize();
        trippin::Rect<int> box{roundedPosition.x - hb.x, roundedPosition.y - hb.y, size.x, size.y};
        if (box.intersect(viewport)) {
            auto frame = (SDL_GetTicks() / sprite->getDuration()) % sprite->getFrames();
            sprite->render(gs.renderer, {box.x - viewport.x, box.y - viewport.y}, frame);
            if (displayLabel) {
                auto posLabel = format(position.x) + ", " + format(position.y);
                auto velLabel = format(velocity.x) + ", " + format(velocity.y);
                auto corner = roundedPosition - viewport.corner();
                gs.fontRenderer->render(std::to_string(id), fontColor, corner + trippin::Point<int>{5, 10});
                gs.fontRenderer->render(posLabel, fontColor, corner + trippin::Point<int>{5, 30});
                gs.fontRenderer->render(velLabel, fontColor, corner + trippin::Point<int>{5, 50});
            }
        }
    }
};

class Ball : public SpriteObject {
public:
    trippin::SpriteManager *spriteManager{};
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

        auto &sprite = spriteManager->get(trippin::SpriteType::ball);
        
        platform = false;
        position.x = 200 + (std::rand() % camera->getUniverse().w / 2);
        position.y = (4880 * mul) - (600 + std::rand() % 100) * mul;
        acceleration = {0, yAccel};
        friction = {xFriction, 0};
        terminalVelocity = {xTerminal, yTerminal};
        platformCollisionType.set(trippin::PlatformCollisionType::reflective);

        SpriteObject::init(&sprite);
    }
};

class Goggin : public SpriteObject {
public:
    trippin::SpriteManager *spriteManager{};
    double xAccel{};
    double yAccel{};
    trippin::Scale scale{};
    double gameTicksPerSecond{};
    double gameTicksPerSecondSq{};

    void init() {
        auto pixelsPerMeter = 240 * scaleMultiplier(scale);
        xAccel = (7.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        yAccel = (8.0 * pixelsPerMeter) / gameTicksPerSecondSq;
        auto xTerminal = (8.0 * pixelsPerMeter) / gameTicksPerSecond;
        auto yTerminal = (35.0 * pixelsPerMeter) / gameTicksPerSecond;

        auto &sprite = spriteManager->get(trippin::SpriteType::goggin);

        auto &hb = sprite.getHitBox();
        platform = false;
        position = {static_cast<double>(hb.corner().x), static_cast<double>(hb.corner().y)};
        acceleration = {0, yAccel};
        terminalVelocity = {xTerminal, yTerminal};

        SpriteObject::init(&sprite);
    }

    void onPlatformCollision(Object &other, const trippin::Sides &collision) override {
        acceleration = {xAccel, yAccel};
    }
};

class Game {
public:
    trippin::Engine engine{};
    trippin::SpriteManager spriteManager{};
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
        auto scale = trippin::Scale::small;
        auto mul = scaleMultiplier(scale);
        auto displayLabel = false;

        spriteManager.setScale(scale);
        spriteManager.load(gs.renderer);
        auto &groundSprite = spriteManager.get(trippin::SpriteType::ground);
        auto &ballSprite = spriteManager.get(trippin::SpriteType::ball);

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

        goggin.displayLabel = displayLabel;
        goggin.scale = scale;
        goggin.setId(nextId++);
        goggin.spriteManager = &spriteManager;
        goggin.gameTicksPerSecond = gameTicksPerSecond;
        goggin.gameTicksPerSecondSq = gameTicksPerSecondSq;
        goggin.init();
        engine.add(&goggin);

        for (int i = 0; i < numGroundPlatforms; i++) {
            auto ground = new SpriteObject;
            ground->setId(nextId++);
            ground->setPlatform(true);
            ground->setPosition(trippin::Point<double>{i * 480.0, 4880} * mul);
            ground->init(&groundSprite);
            grounds.push_back(ground);
            engine.add(ground);
        }

        for (int i = 0; i < numBalls; i++) {
            auto ball = new Ball;
            ball->gameTicksPerSecond = gameTicksPerSecond;
            ball->gameTicksPerSecondSq = gameTicksPerSecondSq;
            ball->displayLabel = displayLabel;
            ball->scale = scale;
            ball->camera = &camera;
            ball->spriteManager = &spriteManager;
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

int main() {
    Game game;
    game.create();
}