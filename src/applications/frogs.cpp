#include "engine/Engine.h"
#include "graphics/RenderableObject.h"
#include "gameloop.h"

void init(int ticksPerFrame, std::vector<trippin::Object *> &objects, const GameState &gs) {
    int ticksSquaredPerFrame = ticksPerFrame * ticksPerFrame;
    bool displayLabels = false;

    trippin::Point<double> terminalVelocity{20.0 / ticksPerFrame, 20.0 / ticksPerFrame};
    trippin::Point<double> friction{0.05 / ticksSquaredPerFrame, 0.05 / ticksSquaredPerFrame};
    trippin::Point<double> gravity{0, 0.2 / ticksSquaredPerFrame};

    int nextId = 1;

    auto player = new trippin::RenderableObject;
    player->setId(nextId++);
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

    auto frog = new trippin::RenderableObject;
    frog->setId(nextId++);
    frog->setPlatform(false);
    frog->setSize({60, 20});
    frog->setPosition({gs.windowSize.x / 2.0 - frog->getSize().x / 2.0, -200});
    frog->setVelocity({0, 0});
    frog->setTerminalVelocity(terminalVelocity);
    frog->setFriction(friction);
    frog->setGravity(gravity);
    frog->setMass(frog->getSize().x * frog->getSize().y);
    frog->color = {0, 0, 128, 255};
    frog->fontColor = {255, 255, 255, 255};
    frog->displayLabel = displayLabels;
    frog->fontRenderer = gs.fontRenderer;
    objects.push_back(frog);

    int wallWidth = 50;

    auto leftWall = new trippin::RenderableObject;
    leftWall->setId(nextId++);
    leftWall->setPlatform(true);
    leftWall->setSize({wallWidth, gs.windowSize.y});
    leftWall->setPosition({0, 0});
    leftWall->color = {0, 0, 0, 255};
    leftWall->fontColor = {255, 255, 255, 255};
    leftWall->displayLabel = displayLabels;
    leftWall->fontRenderer = gs.fontRenderer;
    objects.push_back(leftWall);

    auto rightWall = new trippin::RenderableObject;
    rightWall->setId(nextId++);
    rightWall->setPlatform(true);
    rightWall->setSize({wallWidth, gs.windowSize.y});
    rightWall->setPosition({static_cast<double>(gs.windowSize.x - wallWidth), 0});
    rightWall->color = {0, 0, 0, 255};
    rightWall->fontColor = {255, 255, 255, 255};
    rightWall->displayLabel = displayLabels;
    rightWall->fontRenderer = gs.fontRenderer;
    objects.push_back(rightWall);

    auto floor = new trippin::RenderableObject;
    floor->setId(nextId++);
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
            auto frog = new trippin::RenderableObject;
            frog->setId(objects.size() + 1);
            frog->setPlatform(false);
            frog->setSize({60, 20});
            frog->setPosition({static_cast<double>(300 + std::rand() % 1000), -200});
            frog->setVelocity({0, 0});
            frog->setGravity(objects[0]->getGravity());
            frog->setTerminalVelocity(objects[0]->getTerminalVelocity());
            frog->setFriction(objects[0]->getFriction());
            frog->color = {static_cast<Uint8>(std::rand() % 128),
                            static_cast<Uint8>(std::rand() % 128),
                            static_cast<Uint8>(std::rand() % 128),
                            255};
            frog->fontColor = {255, 255, 255, 255};
            frog->displayLabel = reinterpret_cast<trippin::RenderableObject*>(objects[0])->displayLabel;
            frog->fontRenderer = gs.fontRenderer;
            frog->setMass(frog->getSize().x * frog->getSize().y);
            objects.push_back(frog);
            engine.add(frog);
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
            for (int j=1; j<objects.size(); j++) {
                auto frog = objects[j];
                if (frog->getPlatformCollisions().testBottom()) {
                    auto accel = frog->getPosition().x < player->getPosition().x
                                 ? 0.15 / ticksSquaredPerFrame
                                 : -0.15 / ticksSquaredPerFrame;
                    frog->setAcceleration({accel, frog->getAcceleration().y});
                    if (std::rand() % (100 * ticksPerFrame) < 5) {
                        frog->setVelocity({frog->getVelocity().x, -0.5});
                    }
                }
            }

            engine.tick();
        }
    };

    auto renderFn = [&objects](const GameState &gs) {
        auto center = objects[0]->getRoundedCenter();
        trippin::Rect<int> viewport{center.x - gs.windowSize.x / 2, center.y - gs.windowSize.y / 2, gs.windowSize.x, gs.windowSize.y};

        for (auto obj : objects) {
            (reinterpret_cast<trippin::RenderableObject *>(obj))->render(gs.renderer, viewport);
        }
    };

    gameLoop(initFn, updateFn, renderFn);

    return 0;
}