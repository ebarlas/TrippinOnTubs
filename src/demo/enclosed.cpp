#include "engine/Engine.h"
#include "engine/ReflectiveCollision.h"
#include "engine/ElasticCollision1D.h"
#include "graphics/RenderableObject.h"
#include "gameloop.h"

void init(int ticksPerFrame, std::vector<trippin::Object *> &objects, const GameState &gs) {
    int nextId = 1;
    int wallWidth = 50;
    bool displayLabels = true;

    trippin::Point<double> terminalVelocity = {20.0 / ticksPerFrame, 20.0 / ticksPerFrame};

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

    auto ceiling = new trippin::RenderableObject;
    ceiling->setId(nextId++);
    ceiling->setPlatform(true);
    ceiling->setSize({gs.windowSize.x, wallWidth});
    ceiling->setPosition({0, 0});
    ceiling->color = {0, 0, 0, 255};
    ceiling->fontColor = {255, 255, 255, 255};
    ceiling->displayLabel = displayLabels;
    ceiling->fontRenderer = gs.fontRenderer;
    objects.push_back(ceiling);

    int numObjects = 20;
    for (int i = 0; i < numObjects; i++) {
        int width = 50 + std::rand() % 100;
        int height = 50 + std::rand() % 100;
        auto obj = new trippin::RenderableObject();
        obj->setId(nextId++);
        obj->setPlatform(false);
        obj->setSize({width, height});
        obj->setMass(width * height);
        obj->setPosition({static_cast<double>(std::rand() % (gs.windowSize.x - width)),
                          static_cast<double>(std::rand() % (gs.windowSize.y - height))});
        obj->setVelocity({
                                 static_cast<double>((std::rand() % static_cast<int>(terminalVelocity.x / 2 * 1000.0)) /
                                                     1000.0),
                                 static_cast<double>((std::rand() % static_cast<int>(terminalVelocity.y / 2 * 1000.0)) /
                                                     1000.0)});
        obj->setTerminalVelocity(terminalVelocity * 2.0);
        obj->color = {static_cast<Uint8>(std::rand() % 128),
                      static_cast<Uint8>(std::rand() % 128),
                      static_cast<Uint8>(std::rand() % 128),
                      255};
        obj->fontColor = {255, 255, 255, 255};
        obj->displayLabel = displayLabels;
        obj->fontRenderer = gs.fontRenderer;
        objects.push_back(obj);
    }
}

int main() {
    trippin::Engine engine;
    trippin::ReflectiveCollision platformCollision;
    trippin::ElasticCollision1D objectCollision;
    engine.setPlatformCollision(&platformCollision);
    engine.setObjectCollision(&objectCollision);

    int ticksPerFrame = 10;

    std::vector<trippin::Object *> objects;

    auto initFn = [&ticksPerFrame, &objects, &engine](const GameState &gs) {
        init(ticksPerFrame, objects, gs);
        for (auto obj : objects) {
            engine.add(obj);
        }
    };

    auto updateFn = [&ticksPerFrame, &engine](const GameState &gs) {
        for (int i = 0; i < ticksPerFrame; i++) {
            engine.tick(0);
        }
    };

    auto renderFn = [&objects](const GameState &gs) {
        for (auto obj : objects) {
            trippin::Rect<int> viewport{0, 0, gs.windowSize.x, gs.windowSize.y};
            (reinterpret_cast<trippin::RenderableObject *>(obj))->render(gs.renderer, viewport);
        }
    };

    gameLoop(initFn, updateFn, renderFn);

    return 0;
}