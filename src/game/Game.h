#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include <memory>
#include "SDL.h"
#include "engine/PlatformCollision.h"
#include "engine/InelasticCollision.h"
#include "engine/Engine.h"
#include "engine/Point.h"
#include "sprite/SpriteManager.h"
#include "Configuration.h"
#include "Map.h"
#include "Goggin.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        SDL_Window* window;
        SDL_Renderer* renderer;
        Point<int> windowSize;
        Configuration configuration;
        Map map;
        Scale *scale;
        std::unique_ptr<SpriteManager> spriteManager;
        Camera camera;
        Engine engine;
        std::vector<std::unique_ptr<SpriteObject>> objects;
        Goggin *goggin;
        AbsorbentCollision platformCollision;
        InelasticCollision objectCollision;
        void initRuntime();
        void initWindowSize();
        void initWindow();
        void initRenderer();
        void initConfiguration();
        void initScale();
        void initSpriteManager();
        void initCamera();
        void initEngine();
        void renderLoop();

    public:
        Game(std::string configName);
        ~Game();
        void init();
        void start();
    };
}

#endif
