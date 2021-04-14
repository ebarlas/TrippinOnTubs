#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include <vector>
#include <net/Transport.h>
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
#include "ScrollingScoreBoardOverlay.h"
#include "net/StagingArea.h"
#include "SdlSystem.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        std::unique_ptr<SdlSystem> sdlSystem;
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
        ScrollingScoreBoardOverlay allTimeScoresOverlay;
        ScrollingScoreBoardOverlay todayScoresOverlay;
        StagingArea* stagingArea;
        void initSdl();
        void initRand();
        void initConfiguration();
        void initDbSychronizer();
        void initScale();
        void initSpriteManager();
        void initAutoPlay();
        void initOverlays();
        void initLevel();
        void renderLoop();
        std::unique_ptr<Level> nextLevel();
    public:
        Game(std::string configName);
        void init();
        void start();
    };
}

#endif
