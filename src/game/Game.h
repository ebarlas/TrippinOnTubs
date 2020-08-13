#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include <memory>
#include "SDL.h"
#include "engine/Engine.h"
#include "engine/Point.h"
#include "sprite/SpriteManager.h"
#include "Configuration.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        SDL_Window* window;
        SDL_Renderer* renderer;
        Point<int> windowSize;
        Configuration configuration;
        Scale scale;
        SpriteManager spriteManager;
        Engine engine;
        void initRuntime();
        void initWindowSize();
        void initWindow();
        void initRenderer();
        void initConfiguration();
        void initScale();
        void initSpriteManager();

    public:
        Game(std::string configName);
        ~Game();
        void init();
        void start();
    };
}

#endif
