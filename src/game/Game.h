#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include "SDL.h"
#include "engine/Point.h"
#include "sprite/SpriteManager.h"
#include "Configuration.h"
#include "Map.h"
#include "Level.h"
#include "AutoPlay.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        SDL_Window *window;
        SDL_Renderer *renderer;
        Point<int> windowSize;
        Configuration configuration;
        AutoPlay autoPlay;
        Configuration::Scale *scale;
        std::unique_ptr<SpriteManager> spriteManager;
        SoundManager soundManager;
        std::unique_ptr<Level> level;
        bool loadLevel;
        void initRuntime();
        void initWindowSize();
        void initWindow();
        void initRenderer();
        void initMixer();
        void initConfiguration();
        void initScale();
        void initSpriteManager();
        void initAutoPlay();
        void initLevel();
        void renderLoop();
        std::unique_ptr<Level> nextLevel();

    public:
        Game(std::string configName);
        ~Game();
        void init();
        void start();
    };
}

#endif
