#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include <vector>
#include "SDL.h"
#include "engine/Point.h"
#include "engine/Timer.h"
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
#include "net/Transport.h"
#include "net/StagingArea.h"
#include "SdlSystem.h"
#include "net/Logger.h"
#include "UserInput.h"

namespace trippin {
    class Game {
    private:
        enum class State {
            TITLE,
            START_MENU,
            SCORE_MENU,
            TRAINING,
            PLAYING,
            EXTRA_LIFE_DELAY,
            LEVEL_TRANSITION,
            END_MENU,
            NAME_FORM,
            ALL_TIME_SCORES,
            TODAY_SCORES,
            EXIT
        };

        State state;
        int score{};
        int extraLives{1};
        int lastTicks{};
        std::unique_ptr<Timer> timer;
        std::map<int, int> ticksPerFrame;
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> extraLifeTime;
        std::string configName;
        std::unique_ptr<SdlSystem> sdlSystem;
        Point<int> windowSize;
        Point<int> rendererSize;
        Configuration configuration;
        AutoPlay autoPlay;
        std::unique_ptr<Scale> scale;
        std::unique_ptr<SpriteManager> spriteManager;
        std::unique_ptr<SpriteLoader> spriteLoader;
        std::unique_ptr<SpriteLoadTask> spriteLoadTask;
        SoundManager soundManager;
        std::unique_ptr<Level> level;
        unsigned int levelIndex;
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
        std::unique_ptr<Logger> logger;
        void initSdl();
        void initRand();
        void initLogger();
        void initConfiguration();
        void initDbSynchronizer();
        void initScale();
        void initSpriteManager();
        void initAutoPlay();
        void initOverlays();
        void initLevel();
        void initClock();
        void renderLoop();
        std::unique_ptr<Level> nextLevel();
        void advanceLevel();
        void transferSurfaces();
        void logStateChange(const char *prev, const char *next);
        void render();
        void handle(UserInput::Event &event);
        static const char *getSystemName(SDL_Window *window);
        static const char *getRendererName(SDL_Renderer *renderer);
        static std::string format(const std::map<int, int> &map);
    public:
        Game(std::string configName);
        void init();
        void start();
    };
}

#endif
