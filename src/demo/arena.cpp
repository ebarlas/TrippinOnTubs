#include "engine/Engine.h"
#include "engine/AbsorbentCollision.h"
#include "engine/InelasticCollision.h"
#include "graphics/RenderableObject.h"
#include "gameloop.h"

void init(int ticksPerFrame, std::vector<trippin::RenderableObject *> &objects, const GameState &gs) {
    int ticksSquaredPerFrame = ticksPerFrame * ticksPerFrame;
    bool displayLabels = false;

    trippin::Point<double> terminalVelocity{20.0 / ticksPerFrame, 20.0 / ticksPerFrame};
    trippin::Point<double> friction{0.05 / ticksSquaredPerFrame, 0.05 / ticksSquaredPerFrame};
    double gravity = 0.2 / ticksSquaredPerFrame;

    int nextId = 1;

    auto player = new trippin::RenderableObject;
    player->setId(nextId++);
    player->setPlatform(false);
    player->setSize({120, 120});
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

    auto agent = new trippin::RenderableObject;
    agent->setId(nextId++);
    agent->setPlatform(false);
    agent->setSize({120, 120});
    agent->setPosition({gs.windowSize.x / 2.0 - agent->getSize().x / 2.0, -200});
    agent->setVelocity({0, 0});
    agent->setTerminalVelocity(terminalVelocity);
    agent->setFriction(friction);
    agent->setGravity(gravity);
    agent->setMass(agent->getSize().x * agent->getSize().y);
    agent->color = {0, 0, 128, 255};
    agent->fontColor = {255, 255, 255, 255};
    agent->displayLabel = displayLabels;
    agent->fontRenderer = gs.fontRenderer;
    objects.push_back(agent);

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

    auto midFloor = new trippin::RenderableObject;
    midFloor->setId(nextId++);
    midFloor->setPlatform(true);
    midFloor->setSize({gs.windowSize.x - 200 * 2, wallWidth});
    midFloor->setPosition({200, 500});
    midFloor->color = {0, 0, 0, 255};
    midFloor->fontColor = {255, 255, 255, 255};
    midFloor->displayLabel = displayLabels;
    midFloor->fontRenderer = gs.fontRenderer;
    objects.push_back(midFloor);

    auto leftFloor = new trippin::RenderableObject;
    leftFloor->setId(nextId++);
    leftFloor->setPlatform(true);
    leftFloor->setSize({(gs.windowSize.x - 200) / 2, wallWidth});
    leftFloor->setPosition({0, static_cast<double>(gs.windowSize.y - wallWidth)});
    leftFloor->color = {0, 0, 0, 255};
    leftFloor->fontColor = {255, 255, 255, 255};
    leftFloor->displayLabel = displayLabels;
    leftFloor->fontRenderer = gs.fontRenderer;
    objects.push_back(leftFloor);

    auto rightFloor = new trippin::RenderableObject;
    rightFloor->setId(nextId++);
    rightFloor->setPlatform(true);
    rightFloor->setSize({(gs.windowSize.x - 200) / 2, wallWidth});
    rightFloor->setPosition({static_cast<double>(gs.windowSize.x / 2.0 + 100),
                             static_cast<double>(gs.windowSize.y - wallWidth)});
    rightFloor->color = {0, 0, 0, 255};
    rightFloor->fontColor = {255, 255, 255, 255};
    rightFloor->displayLabel = displayLabels;
    rightFloor->fontRenderer = gs.fontRenderer;
    objects.push_back(rightFloor);
}

int main() {
    trippin::Engine engine;
    trippin::AbsorbentCollision platformCollision;
    trippin::InelasticCollision objectCollision;
    engine.setPlatformCollision(&platformCollision);
    engine.setObjectCollision(&objectCollision);

    int ticksPerFrame = 10;
    int ticksSquaredPerFrame = ticksPerFrame * ticksPerFrame;

    std::vector<trippin::RenderableObject *> objects;
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
        Uint32 now = SDL_GetTicks();
        if (now - lastTime > 1000) {
            int width = 50 + std::rand() % 100;
            int height = 50 + std::rand() % 100;
            auto obj = new trippin::RenderableObject;
            obj->setId(objects.size() + 1);
            obj->setPlatform(false);
            obj->setSize({width, height});
            obj->setPosition({static_cast<double>(300 + std::rand() % 1000), -200});
            obj->setVelocity({0, 0});
            obj->setGravity(objects[0]->getGravity());
            obj->setTerminalVelocity(objects[0]->getTerminalVelocity());
            obj->setFriction(objects[0]->getFriction());
            obj->color = {static_cast<Uint8>(std::rand() % 128),
                          static_cast<Uint8>(std::rand() % 128),
                          static_cast<Uint8>(std::rand() % 128),
                          255};
            obj->fontColor = {255, 255, 255, 255};
            obj->displayLabel = reinterpret_cast<trippin::RenderableObject*>(objects[0])->displayLabel;
            obj->fontRenderer = gs.fontRenderer;
            obj->setMass(width * height);
            objects.push_back(obj);
            engine.add(obj);
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
        objects[0]->setAcceleration(a);

        for (int i = 0; i < ticksPerFrame; i++) {
            engine.tick(0);
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