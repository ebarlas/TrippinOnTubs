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
#include "Spirit.h"
#include "SpiritClock.h"
#include "JumpMeter.h"
#include "Score.h"
#include "Activation.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        SDL_Window* window;
        SDL_Renderer* renderer;
        Point<int> windowSize;
        Configuration configuration;
        Map map;
        Configuration::Scale *scale;
        std::unique_ptr<SpriteManager> spriteManager;
        Camera camera;
        Engine engine;
        Activation activation;
        std::vector<std::unique_ptr<Renderable>> objects;
        Goggin goggin;
        AbsorbentCollision platformCollision;
        InelasticCollision objectCollision;
        Spirit spirit;
        SpiritClock spiritClock;
        JumpMeter jumpMeter;
        Score score;
        void initRuntime();
        void initWindowSize();
        void initWindow();
        void initRenderer();
        void initConfiguration();
        void initMap();
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
