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
#include "LifeMeter.h"
#include "TrainingProgram.h"
#include "SceneBuilder.h"
#include "sprite/RenderClock.h"

namespace trippin {
    class Level : public Listener {
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
        std::vector<std::unique_ptr<Listener>> objects;
        const std::vector<GogginInputTick> *autoPlay{};
        std::unique_ptr<Goggin> goggin;
        Spirit spirit;
        std::unique_ptr<SpiritClock> spiritClock;
        std::unique_ptr<JumpMeter> jumpMeter;
        int initialScore{};
        std::unique_ptr<ScoreTicker> scoreTicker;
        int initialExtraLives{};
        std::unique_ptr<LifeMeter> lifeMeter;
        bool training;
        std::unique_ptr<TrainingProgram> trainingProgram;
        const RenderClock *renderClock;
        SceneBuilder sceneBuilder;
        void initMap();
        void initCamera();
        void initEngine();
    public:
        void setWindowSize(Point<int> windowSize);
        void setConfiguration(Configuration *configuration);
        void setScale(Configuration::Scale *scale);
        void setSpriteManager(SpriteManager *spriteManager);
        void setSoundManager(SoundManager *soundManager);
        void setRenderClock(const RenderClock &renderClock);
        void setAutoPlay(const std::vector<GogginInputTick> &autoPlay);
        void setMapName(const std::string &name);
        void setScore(int score);
        void setExtraLives(int extraLives);
        void setTraining(bool training);
        void init();
        bool ended();
        bool completed();
        void onInput(GogginInput input);
        void render();
        void start();
        void stop();
        void pause();
        void resume();
        int getScore() const;
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif
