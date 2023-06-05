#ifndef TRIPPIN_TRAININGPROGRAM_H
#define TRIPPIN_TRAININGPROGRAM_H

#include <array>
#include <atomic>
#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "Goggin.h"
#include "ui/Interpolator.h"
#include "LevelStats.h"

namespace trippin {
    class TrainingProgram : public Listener {
    private:
        static constexpr LevelStats::Event STAGES[]{
                LevelStats::Event::Jump,
                LevelStats::Event::ChargedJump,
                LevelStats::Event::DoubleJump,
                LevelStats::Event::Duck,
                LevelStats::Event::JumpSlamDown,
                LevelStats::Event::DuckJump};

        static constexpr const char *NAMES[]{
                "jump",
                "charged_jump",
                "double_jump",
                "duck",
                "jump_slam_down",
                "charged_duck_jump"};

        static constexpr const int MOVES_PER_STAGE = 3;
        static constexpr const int TUBS_PER_JUMP = 4;

        const Sprite &titleSprite;
        const Sprite &controlSprite;
        const Sprite &statusSprite;

        const LevelStats &stats;
        const Point<int> windowSize;
        const int margin;
        const unsigned int stage;
        Interpolator titleInterpolator;
        Interpolator controlInterpolator;
        const int finishedWaitTicks;
        int stageTicks;
        Mix_Chunk *const sound;

        SceneBuilder &sceneBuilder;

        std::atomic_bool complete;
        bool firstTick;
        bool stageDone;

        const int startProgress;
        int progress;
        std::atomic_int trainingProgress;

        int lastTubTime;
        int tubCount;

        void slideIn();
        void slideOut();
    public:
        TrainingProgram(
                Point<int> windowSize,
                int margin,
                const Configuration &configuration,
                SpriteManager &spriteManager,
                SoundManager &soundManager,
                const LevelStats &stats,
                const RenderClock &renderClock,
                SceneBuilder &sb,
                unsigned int stage,
                int progress);
        bool completed();
        int getProgress() const;
        void afterTick(int engineTicks) override;
    };
}

#endif
