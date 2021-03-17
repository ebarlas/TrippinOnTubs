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
#include "TitleOverlay.h"
#include "MenuOverlay.h"
#include "EndMenuOverlay.h"
#include "NameFormOverlay.h"
#include "ScoreMenuOverlay.h"

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
        TitleOverlay titleOverlay;
        MenuOverlay menuOverlay;
        EndMenuOverlay endMenuOverlay;
        NameFormOverlay nameFormOverlay;
        ScoreMenuOverlay scoreMenuOverlay;
        void initRuntime();
        void initWindowSize();
        void initWindow();
        void initRenderer();
        void initMixer();
        void initConfiguration();
        void initScale();
        void initSpriteManager();
        void initAutoPlay();
        void initOverlays();
        void initLevel();
        void renderLoop();
        std::unique_ptr<Level> nextLevel();

        struct UserInput {
            bool quit;
            bool spaceKeyDown;
            bool downKeyDown;
            bool spaceKeyUp;
            bool downKeyUp;
            bool mouseButtonDown;
            Point<int> mouseButton;
        };

        static UserInput pollEvents();
        static GogginInput getGogginInput(const UserInput &userInput);

    public:
        Game(std::string configName);
        ~Game();
        void init();
        void start();
    };
}

#endif
