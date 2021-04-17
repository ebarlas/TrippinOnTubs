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
#include "ScoreTicker.h"
#include "Activation.h"
#include "GogginInput.h"

namespace trippin {
    class Level {
    private:
        Point<int> windowSize;
        Configuration *configuration;
        Configuration::Scale *scale;
        SpriteManager *spriteManager;
        SoundManager *soundManager;
        const std::string *mapName;
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
        ScoreTicker scoreTicker;
        void initMap();
        void initCamera();
        void initEngine();
    public:
        void setWindowSize(Point<int> windowSize);
        void setConfiguration(Configuration *configuration);
        void setScale(Configuration::Scale *scale);
        void setSpriteManager(SpriteManager *spriteManager);
        void setSoundManager(SoundManager *soundManager);
        void setAutoPlay(const std::vector<GogginInputTick> &autoPlay);
        void setMapName(const std::string &name);
        void init();
        bool ended();
        void render(GogginInput input);
        void start();
        void stop();
        void pause();
        void resume();
        int getScore() const;
    };
}

#endif
