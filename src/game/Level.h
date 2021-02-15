#ifndef TRIPPIN_LEVEL_H
#define TRIPPIN_LEVEL_H

#include "engine/Engine.h"
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
    class Level {
    private:
        Point<int> windowSize;
        Configuration *configuration;
        Configuration::Scale *scale;
        SpriteManager *spriteManager;
        Map map;
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
        void initMap();
        void initCamera();
        void initEngine();
    public:
        struct Input {
            bool spaceKeyDown;
            bool spaceKeyUp;
            bool downKeyDown;
            bool downKeyUp;
        };

        void setWindowSize(Point<int> windowSize);
        void setConfiguration(Configuration *configuration);
        void setScale(Configuration::Scale *scale);
        void setSpriteManager(SpriteManager *spriteManager);
        void init();
        void render(Input input);
        void start();
    };
}

#endif
