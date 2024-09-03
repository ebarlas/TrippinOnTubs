#ifndef TRIPPIN_GAME_H
#define TRIPPIN_GAME_H

#include <string>
#include <vector>
#include "SDL.h"
#include "engine/Point.h"
#include "engine/Timer.h"
#include "engine/ClockStats.h"
#include "sprite/SpriteManager.h"
#include "Configuration.h"
#include "Map.h"
#include "Level.h"
#include "AutoPlay.h"
#include "ui/TitleOverlay.h"
#include "ui/MenuLayout.h"
#include "ui/NameForm.h"
#include "ui/ScrollingScoreBoard.h"
#include "ui/SimpleOverlay.h"
#include "ui/ExitOverlay.h"
#include "ui/SpeedUpOverlay.h"
#include "ui/Marquee.h"
#include "net/Transport.h"
#include "net/StagingArea.h"
#include "SdlSystem.h"
#include "net/Logger.h"
#include "UserInput.h"
#include "Random.h"
#include "MyScores.h"
#include "Db.h"

namespace trippin {
    class Game {
    private:
        enum class State {
            TITLE,
            START_MENU,
            SCORE_MENU,
            TRAINING,
            TRAINING_COMPLETED,
            PLAYING,
            EXTRA_LIFE_DELAY,
            LEVEL_TRANSITION,
            GAME_OVER,
            LEVELS_COMPLETED,
            END_MENU,
            NAME_FORM,
            ALL_TIME_SCORES,
            TODAY_SCORES,
            REPLAY,
            EXIT,
            MY_SCORES_MENU,
            MY_TOP_SCORES,
            MY_LATEST_SCORES
        };

        State state;
        int score{};
        unsigned int trainingStage{};
        int trainingProgress{};
        std::vector<std::vector<GogginInputTick>> inputEvents;
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
        std::unique_ptr<TitleOverlay> titleOverlay;
        std::unique_ptr<MenuLayout<4>> titleMenu;
        std::unique_ptr<ExitOverlay> replayExitOverlay;
        std::unique_ptr<ExitOverlay> trainingExitOverlay;
        std::unique_ptr<ExitOverlay> playingExitOverlay;
        std::unique_ptr<SpeedUpOverlay> speedUpOverlay;
        std::unique_ptr<MenuLayout<2>> endMenu;
        std::unique_ptr<NameForm> nameForm;
        std::unique_ptr<MenuLayout<3>> scoreMenu;
        std::unique_ptr<MenuLayout<3>> myScoresMenu;
        std::unique_ptr<ScrollingScoreBoard> topScoreBoard;
        std::unique_ptr<SimpleOverlay> levelOverlay;
        std::unique_ptr<SimpleOverlay> gameOverOverlay;
        std::unique_ptr<SimpleOverlay> levelsCompletedOverlay;
        std::unique_ptr<SimpleOverlay> trainingCompletedOverlay;
        std::unique_ptr<Marquee> marquee;
        std::unique_ptr<StagingArea> stagingArea;
        std::unique_ptr<MyScores> myScores;
        std::unique_ptr<Transport> transport;
        std::unique_ptr<Db<Score>> scoreDb;
        std::unique_ptr<Db<LogEvent>> logDb;
        Rect<int> scoreArea;
        Rect<int> flagsArea;
        RenderClock renderClock;
        std::unique_ptr<Logger> logger;
        Random<int, 0, 10'000'000> random;
        std::string appId;
        int gameId;
        Score replayScore;
        unsigned int replayOffset;
        std::vector<GogginInputTick> replayAutoPlay;
        ClockStats fpsStats;
        void initSdl();
        void initAppId();
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
        std::vector<std::vector<Score::InputEvent>> convertInputEvents() const;
        std::string formatFps() const;
        std::string formatTps() const;
        static std::string formatTps(int min, int max, int avg);
        static std::vector<GogginInputTick> convertEvents(const std::vector<Score::InputEvent> &events) ;
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
