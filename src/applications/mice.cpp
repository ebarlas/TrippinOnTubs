#include "engine/Engine.h"
#include "graphics/RenderableObject.h"
#include "gameloop.h"

class Mouse : public trippin::RenderableObject {
public:
    struct Collision {
        trippin::Object *obj{};
        trippin::Sides sides{};
    };

    std::vector<Collision> collisions{};

    void clear() {
        collisions.clear();
    }

    void onPlatformCollision(Object &other, const trippin::Sides &collision) override {

    }

    void onObjectCollision(Object &other, const trippin::Sides &collision) override {
        if (other.getId() >= 2) {
            collisions.push_back({&other, collision});
        }
    }
};

void init(int ticksPerFrame, std::vector<trippin::Object *> &objects, const GameState &gs) {
    int ticksSquaredPerFrame = ticksPerFrame * ticksPerFrame;
    bool displayLabels = false;

    trippin::Point<double> terminalVelocity{20.0 / ticksPerFrame, 20.0 / ticksPerFrame};
    trippin::Point<double> friction{0.05 / ticksSquaredPerFrame, 0.05 / ticksSquaredPerFrame};
    double gravity = 0.2 / ticksSquaredPerFrame;

    auto player = new trippin::RenderableObject;
    player->setId(1);
    player->setPlatform(false);
    player->setSize({80, 200});
    player->setPosition({gs.windowSize.x / 2.0 - player->getSize().x / 2.0, 0});
    player->setVelocity({0, 0});
    player->setTerminalVelocity(terminalVelocity);
    player->setFriction(friction);
    player->setGravity(gravity);
    player->setMass(player->getSize().x * player->getSize().y);
    player->color = {0, 0, 255, 255};
    player->fontColor = {255, 255, 255, 255};
    player->displayLabel = displayLabels;
    player->fontRenderer = gs.fontRenderer;
    objects.push_back(player);

    auto mouse = new Mouse;
    mouse->setId(2);
    mouse->setPlatform(false);
    mouse->setSize({60, 20});
    mouse->setPosition({gs.windowSize.x / 2.0 - mouse->getSize().x / 2.0, -200});
    mouse->setVelocity({0, 0});
    mouse->setTerminalVelocity(terminalVelocity);
    mouse->setFriction(friction);
    mouse->setGravity(gravity);
    mouse->setMass(mouse->getSize().x * mouse->getSize().y);
    mouse->color = {0, 0, 128, 255};
    mouse->fontColor = {255, 255, 255, 255};
    mouse->displayLabel = displayLabels;
    mouse->fontRenderer = gs.fontRenderer;
    objects.push_back(mouse);

    int wallWidth = 50;

    auto leftWall = new trippin::RenderableObject;
    leftWall->setId(0);
    leftWall->setPlatform(true);
    leftWall->setSize({wallWidth, gs.windowSize.y});
    leftWall->setPosition({0, 0});
    leftWall->color = {0, 0, 0, 255};
    leftWall->fontColor = {255, 255, 255, 255};
    leftWall->displayLabel = displayLabels;
    leftWall->fontRenderer = gs.fontRenderer;
    objects.push_back(leftWall);

    auto rightWall = new trippin::RenderableObject;
    rightWall->setId(0);
    rightWall->setPlatform(true);
    rightWall->setSize({wallWidth, gs.windowSize.y});
    rightWall->setPosition({static_cast<double>(gs.windowSize.x - wallWidth), 0});
    rightWall->color = {0, 0, 0, 255};
    rightWall->fontColor = {255, 255, 255, 255};
    rightWall->displayLabel = displayLabels;
    rightWall->fontRenderer = gs.fontRenderer;
    objects.push_back(rightWall);

    auto floor = new trippin::RenderableObject;
    floor->setId(0);
    floor->setPlatform(true);
    floor->setSize({gs.windowSize.x, wallWidth});
    floor->setPosition({0, static_cast<double>(gs.windowSize.y - wallWidth)});
    floor->color = {0, 0, 0, 255};
    floor->fontColor = {255, 255, 255, 255};
    floor->displayLabel = displayLabels;
    floor->fontRenderer = gs.fontRenderer;
    objects.push_back(floor);
}

int main() {
    trippin::Engine engine;
    engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
    engine.setObjectCollisionType(trippin::ObjectCollisionType::inelastic);

    int ticksPerFrame = 10;
    int ticksSquaredPerFrame = ticksPerFrame * ticksPerFrame;

    std::vector<trippin::Object *> objects;
    Uint32 lastTime{};

    auto initFn = [&lastTime, &ticksPerFrame, &objects, &engine](const GameState &gs) {
        init(ticksPerFrame, objects, gs);
        lastTime = SDL_GetTicks();
        for (auto obj : objects) {
            engine.add(obj);
        }
    };

    auto updateFn = [&lastTime, &ticksPerFrame, &ticksSquaredPerFrame, &objects, &engine](
            const GameState &gs) {
        auto player = objects[0];

        Uint32 now = SDL_GetTicks();
        if (now - lastTime > 2000) {
            auto mouse = new Mouse;
            mouse->setId(objects.size() - 2);
            mouse->setPlatform(false);
            mouse->setSize({60, 20});
            mouse->setPosition({static_cast<double>(300 + std::rand() % 1000), -200});
            mouse->setVelocity({0, 0});
            mouse->setGravity(objects[0]->getGravity());
            mouse->setTerminalVelocity(objects[0]->getTerminalVelocity());
            mouse->setFriction(objects[0]->getFriction());
            mouse->color = {static_cast<Uint8>(std::rand() % 128),
                            static_cast<Uint8>(std::rand() % 128),
                            static_cast<Uint8>(std::rand() % 128),
                            255};
            mouse->fontColor = {255, 255, 255, 255};
            mouse->displayLabel = reinterpret_cast<trippin::RenderableObject *>(objects[0])->displayLabel;
            mouse->fontRenderer = gs.fontRenderer;
            mouse->setMass(mouse->getSize().x * mouse->getSize().y);
            objects.push_back(mouse);
            engine.add(mouse);
            lastTime = now;
        }

        trippin::Point<double> a{};
        if (gs.keysDown.testTop()) {
            a.y = -0.6 / ticksSquaredPerFrame;
        } else if (gs.keysDown.testBottom()) {
            a.y = 0.6 / ticksSquaredPerFrame;
        } else {
            a.y = 0.0 / ticksSquaredPerFrame;
        }
        if (gs.keysDown.testLeft()) {
            a.x = -0.4 / ticksSquaredPerFrame;
        } else if (gs.keysDown.testRight()) {
            a.x = 0.4 / ticksSquaredPerFrame;
        } else {
            a.x = 0;
        }
        player->setAcceleration(a);

        for (int i = 0; i < ticksPerFrame; i++) {
            for (int j = 0; j < objects.size(); j++) {
                if (objects[j]->getId() >= 2) {
                    auto mouse = reinterpret_cast<Mouse *>(objects[j]);
                    if (mouse->getPlatformCollisions().testBottom() || mouse->getObjectCollisions().testBottom()) {
                        double x;
                        if (mouse->getPosition().x < player->getPosition().x) {
                            x = 0.1 / ticksSquaredPerFrame;
                        } else {
                            x = -0.1 / ticksSquaredPerFrame;
                        }
                        mouse->setAcceleration({x, mouse->getAcceleration().y});
                    }

                    bool anyMouse = false;
                    if (mouse->getPosition().x < player->getPosition().x) {
                        for (int k = 0; k < mouse->collisions.size(); k++) {
                            if (mouse->collisions[k].sides.testRight()) {
                                anyMouse = true;
                                break;
                            }
                        }
                    } else {
                        for (int k = 0; k < mouse->collisions.size(); k++) {
                            if (mouse->collisions[k].sides.testLeft()) {
                                anyMouse = true;
                                break;
                            }
                        }
                    }
                    if (anyMouse) {
                        mouse->setVelocity({mouse->getVelocity().x, -0.2});
                    }
                }
            }

            for (int j = 1; j < objects.size(); j++) {
                auto mouse = objects[j];
                if (mouse->getId() >= 2) {
                    reinterpret_cast<Mouse *>(mouse)->clear();
                }
            }

            engine.tick({0});
        }
    };

    auto renderFn = [&objects](const GameState &gs) {
        auto center = objects[0]->getRoundedCenter();
        trippin::Rect<int> viewport{center.x - gs.windowSize.x / 2, center.y - gs.windowSize.y / 2, gs.windowSize.x,
                                    gs.windowSize.y};

        for (auto obj : objects) {
            (reinterpret_cast<trippin::RenderableObject *>(obj))->render(gs.renderer, viewport);
        }
    };

    gameLoop(initFn, updateFn, renderFn);

    return 0;
}