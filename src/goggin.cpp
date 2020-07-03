#include <sprite/Camera.h>
#include <sstream>
#include <iomanip>
#include "sprite/SpriteManager.h"
#include "engine/Engine.h"
#include "gameloop.h"

constexpr bool displayLabel = true;
constexpr SDL_Color fontColor{100, 100, 100, 255};

std::string format(double d, int precision = 2) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << (d >= 0 ? "" : "_") << std::abs(d);
    return stream.str();
}

class SpriteObject : public trippin::Object {
public:
    const trippin::Sprite *sprite{};
    Uint32 ticks{};

    void init(const trippin::Sprite *sp) {
        sprite = sp;
        auto &hb = sprite->getHitBox();
        size = {hb.w, hb.h};
        mass = hb.area();
        updateRounded();
    }

    void tick() {
        ticks++;
    }

    void render(const GameState &gs, const trippin::Camera &camera) {
        auto &hb = sprite->getHitBox();
        auto &viewport = camera.getViewport();
        auto &size = sprite->getSize();
        trippin::Rect<int> box{roundedPosition.x - hb.x, roundedPosition.y - hb.y, size.x, size.y};
        if (box.intersect(viewport)) {
            auto frame = (ticks / sprite->getDuration()) % sprite->getFrames();
            sprite->render(gs.renderer, {box.x - viewport.x, box.y - viewport.y}, frame);
            if (displayLabel) {
                auto posLabel = format(position.x) + ", " + format(position.y);
                auto velLabel = format(velocity.x) + ", " + format(velocity.y);
                gs.fontRenderer->render(std::to_string(id), fontColor,
                                        roundedPosition - viewport.corner() + trippin::Vector<int>{5, 10});
                gs.fontRenderer->render(posLabel, fontColor,
                                        roundedPosition - viewport.corner() + trippin::Vector<int>{5, 30});
                gs.fontRenderer->render(velLabel, fontColor,
                                        roundedPosition - viewport.corner() + trippin::Vector<int>{5, 50});
            }
        }
    }
};

class Game {
public:
    trippin::Engine engine{};
    trippin::SpriteManager spriteManager{};
    SpriteObject goggin{};
    std::vector<SpriteObject *> grounds{};
    std::vector<SpriteObject *> balls{};
    trippin::Camera camera{};
    Uint32 totalTicks{};

    void create() {
        auto initFn = [this](const GameState &gs) {
            init(gs);
        };

        auto updateFn = [this](const GameState &gs) {
            update(gs);
        };

        auto renderFn = [this](const GameState &gs) {
            render(gs);
        };

        gameLoop(initFn, updateFn, renderFn);
    }

    void init(const GameState &gs) {
        auto scale = trippin::Scale::small;
        auto mul = scaleMultiplier(scale);

        spriteManager.setScale(scale);
        spriteManager.load(gs.renderer);
        auto &gogginSprite = spriteManager.get(trippin::SpriteType::goggin);
        auto &groundSprite = spriteManager.get(trippin::SpriteType::ground);
        auto &ballSprite = spriteManager.get(trippin::SpriteType::ball);

        int numGroundPlatforms = 20;
        int numBalls = 20;

        camera.setViewport({0, 0, gs.windowSize.x, gs.windowSize.y});
        camera.setUniverse({0, 0,
                            numGroundPlatforms * groundSprite.getSize().x,
                            static_cast<int>(8000 * mul)});

        engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
        engine.setObjectCollisionType(trippin::ObjectCollisionType::inelastic);

        int nextId = 1;

        auto pixelsPerMeter = 240 * mul;
        auto ticksPerSecond = 1000;
        auto ticksPerSecondSq = ticksPerSecond * ticksPerSecond;
        auto yAccel = (8.0 * pixelsPerMeter) / ticksPerSecondSq;
        auto xTerminalGoggin = (8.0 * pixelsPerMeter) / ticksPerSecond;
        auto xTerminalBall = (10.0 * pixelsPerMeter) / ticksPerSecond;
        auto yTerminal = (35.0 * pixelsPerMeter) / ticksPerSecond;
        auto xFrictionBall = (1.0 * pixelsPerMeter) / ticksPerSecondSq;

        auto &hb = gogginSprite.getHitBox();
        goggin.setId(nextId++);
        goggin.setPlatform(false);
        goggin.setPosition({static_cast<double>(hb.corner().x), static_cast<double>(hb.corner().y)});
        goggin.setAcceleration({0, yAccel});
        goggin.setTerminalVelocity({xTerminalGoggin, yTerminal});
        goggin.init(&gogginSprite);
        engine.add(&goggin);

        for (int i = 0; i < numGroundPlatforms; i++) {
            auto ground = new SpriteObject;
            ground->setId(nextId++);
            ground->setPlatform(true);
            ground->setPosition(trippin::Vector<double>{i * 480.0, 4880} * mul);
            ground->init(&groundSprite);
            grounds.push_back(ground);
            engine.add(ground);
        }

        for (int i = 0; i < numBalls; i++) {
            auto ball = new SpriteObject;
            double x = 200 + (std::rand() % camera.getUniverse().w / 2);
            double y = (4880 * mul) - (300 + std::rand() % 100);
            ball->setId(nextId++);
            ball->setPlatform(false);
            ball->setPlatformCollisionType(trippin::PlatformCollisionType::reflective);
            ball->setPosition({x, y});
            ball->setFriction({xFrictionBall, 0});
            ball->setAcceleration({0, yAccel});
            ball->setTerminalVelocity({xTerminalBall, yTerminal});
            ball->init(&ballSprite);
            balls.push_back(ball);
            engine.add(ball);
        }
    }

    void update(const GameState &gs) {
        auto scale = trippin::Scale::small;
        auto mul = scaleMultiplier(scale);
        auto pixelsPerMeter = 240 * mul;
        auto ticksPerSecond = 1000;
        auto ticksPerSecondSq = ticksPerSecond * ticksPerSecond;
        auto xAccel = (7.0 * pixelsPerMeter) / ticksPerSecondSq;
        auto yAccel = (8.0 * pixelsPerMeter) / ticksPerSecondSq;

        for (int i = 0; i < gs.ticks; i++) {
            if (goggin.getPlatformCollisions().testBottom()) {
                goggin.setAcceleration({xAccel, yAccel});
            } else {
                goggin.setAcceleration({0, yAccel});
            }

            engine.tick();
            goggin.tick();
        }

        totalTicks += gs.ticks;
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
        if (displayLabel) {
            gs.fontRenderer->render(std::to_string(SDL_GetTicks()), fontColor, {5, 10});
            gs.fontRenderer->render(std::to_string(totalTicks), fontColor, {5, 30});
        }
    }
};

int main() {
    Game game;
    game.create();
}