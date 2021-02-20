#ifndef TRIPPIN_LEVEL_H
#define TRIPPIN_LEVEL_H

#include <SDL_mixer.h>
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
#include "UserInput.h"

namespace trippin {
    class Level {
    private:
        Point<int> windowSize;
        Configuration *configuration;
        Configuration::Scale *scale;
        SpriteManager *spriteManager;
        SoundManager *soundManager;
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
        void setWindowSize(Point<int> windowSize);
        void setConfiguration(Configuration *configuration);
        void setScale(Configuration::Scale *scale);
        void setSpriteManager(SpriteManager *spriteManager);
        void setSoundManager(SoundManager *soundManager);
        void init();
        bool ended();
        void render(UserInput input);
        void start();
        void stop();
    };
}

#endif
