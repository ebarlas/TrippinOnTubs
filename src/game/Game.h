#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include "SDL.h"
#include "engine/Point.h"
#include "sprite/SpriteManager.h"
#include "Configuration.h"
#include "Map.h"
#include "Level.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        SDL_Window *window;
        SDL_Renderer *renderer;
        Point<int> windowSize;
        Configuration configuration;
        Configuration::Scale *scale;
        std::unique_ptr<SpriteManager> spriteManager;
        Level level;
        void initRuntime();
        void initWindowSize();
        void initWindow();
        void initRenderer();
        void initConfiguration();
        void initScale();
        void initSpriteManager();
        void initLevel();
        void renderLoop();

    public:
        Game(std::string configName);
        ~Game();
        void init();
        void start();
    };
}

#endif
