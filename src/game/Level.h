#ifndef TRIPPIN_LEVEL_H
#define TRIPPIN_LEVEL_H

#include <SDL_mixer.h>
#include "engine/Engine.h"
#include "sprite/SpriteManager.h"
#include "Configuration.h"
#include "Map.h"
#include "Goggin.h"
#include "GogginRenderer.h"
#include "Spirit.h"
#include "SpiritClock.h"
#include "JumpMeter.h"
#include "ScoreTicker.h"
#include "Activation.h"
#include "GogginInput.h"
#include "LifeMeter.h"
#include "ComboManager.h"
#include "NotificationManager.h"
#include "NotificationDrawer.h"
#include "TrainingProgram.h"
#include "SceneBuilder.h"
#include "sprite/RenderClock.h"
#include "GroupManager.h"
#include "LevelBonuses.h"
#include "LevelEndListener.h"
#include "LevelStats.h"

namespace trippin {
    class Level : public Listener {
    private:
        Point<int> windowSize;
        Configuration *configuration;
        const Scale* scale;
        SpriteManager *spriteManager;
        SoundManager *soundManager;
        const std::string *mapName;
        Map map;
        std::unique_ptr<Camera> camera;
        Engine engine;
        std::vector<std::unique_ptr<Listener>> objects;
        const std::vector<GogginInputTick> *autoPlay{};
        std::unique_ptr<PointCloudManager> pointCloudManager;
        std::unique_ptr<Goggin> goggin;
        std::unique_ptr<GogginRenderer> gogginRenderer;
        Spirit spirit;
        std::unique_ptr<SpiritClock> spiritClock;
        std::unique_ptr<JumpMeter> jumpMeter;
        int initialScore{};
        std::unique_ptr<ScoreTicker> scoreTicker;
        int initialExtraLives{};
        std::unique_ptr<LifeMeter> lifeMeter;
        bool training;
        unsigned int trainingStage;
        bool lastLevel;
        std::unique_ptr<TrainingProgram> trainingProgram;
        const RenderClock *renderClock;
        SceneBuilder sceneBuilder;
        std::unique_ptr<ComboManager> comboManager;
        std::unique_ptr<NotificationManager> comboNotificationManager;
        std::unique_ptr<NotificationManager> groupNotificationManager;
        std::unique_ptr<NotificationDrawer> notificationDrawer;
        std::unique_ptr<LevelBonuses> levelBonuses;
        std::unique_ptr<LevelEndListener> levelEndListener;
        std::unique_ptr<LevelStats> levelStats;
        GroupManager groupManager;
        void initMap();
        void initCamera();
        void initEngine();
    public:
        void setWindowSize(Point<int> windowSize);
        void setConfiguration(Configuration *configuration);
        void setScale(const Scale* scale);
        void setSpriteManager(SpriteManager *spriteManager);
        void setSoundManager(SoundManager *soundManager);
        void setRenderClock(const RenderClock &renderClock);
        void setAutoPlay(const std::vector<GogginInputTick> &autoPlay);
        void setMapName(const std::string &name);
        void setScore(int score);
        void setExtraLives(int extraLives);
        void setTraining(unsigned int stage);
        unsigned int getTrainingStage() const;
        void setLastLevel(bool lastLevel);
        void init();
        bool ended();
        bool completed();
        bool trainingCompleted() const;
        void onInput(GogginInput input, bool replay);
        void render();
        void start();
        void stop();
        void pause();
        void resume();
        int getScore() const;
        std::vector<GogginInputTick> takeInputEvents() const;
        int getTicks() const;
        int getMinTps() const;
        int getMaxTps() const;
        int getAvgTps() const;
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
    };
}

#endif
