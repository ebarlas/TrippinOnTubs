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
#include "ui/TitleOverlay.h"
#include "ui/TitleMenu.h"
#include "ui/EndMenu.h"
#include "ui/NameForm.h"
#include "ui/ScoreMenu.h"
#include "ui/ScrollingScoreBoard.h"
#include "ui/LevelOverlay.h"
#include "net/StagingArea.h"
#include "SdlSystem.h"

namespace trippin {
    class Game {
    private:
        std::string configName;
        std::unique_ptr<SdlSystem> sdlSystem;
        Point<int> windowSize;
        Point<int> rendererSize;
        Configuration configuration;
        AutoPlay autoPlay;
        Configuration::Scale *scale;
        std::unique_ptr<SpriteManager> spriteManager;
        std::unique_ptr<SpriteLoader> spriteLoader;
        std::unique_ptr<SpriteLoadTask> spriteLoadTask;
        SoundManager soundManager;
        std::unique_ptr<Level> level;
        int levelIndex;
        bool loadLevel;
        bool trainLevel;
        std::unique_ptr<TitleOverlay> titleOverlay;
        std::unique_ptr<TitleMenu> titleMenu;
        std::unique_ptr<EndMenu> endMenu;
        std::unique_ptr<NameForm> nameForm;
        std::unique_ptr<ScoreMenu> scoreMenu;
        std::unique_ptr<ScrollingScoreBoard> topScoreBoard;
        std::unique_ptr<ScrollingScoreBoard> todayScoreBoard;
        std::unique_ptr<LevelOverlay> levelOverlay;
        std::shared_ptr<StagingArea> stagingArea;
        RenderClock renderClock;
        void initSdl();
        void initRand();
        void initConfiguration();
        void initDbSychronizer();
        void initScale();
        void initSpriteManager();
        void initAutoPlay();
        void initOverlays();
        void initLevel();
        void initClock();
        void renderLoop();
        std::unique_ptr<Level> nextLevel();
        void advanceLevel(int score, int extraLives);
        void transferSurfaces();
    public:
        Game(std::string configName);
        void init();
        void start();
    };
}

#endif
