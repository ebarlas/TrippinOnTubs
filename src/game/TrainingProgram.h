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
                LevelStats::Event::Duck,
                LevelStats::Event::Stop,
                LevelStats::Event::ChargedJump,
                LevelStats::Event::DuckJump,
                LevelStats::Event::DoubleJump,
                LevelStats::Event::JumpSlamDown};

        static constexpr const char *NAMES[]{
                "jump",
                "duck",
                "stop",
                "charged_jump",
                "charged_duck_jump",
                "double_jump",
                "jump_slam_down"};

        const Sprite &titleSprite;
        const Sprite &controlSprite;

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

        void resetInterpolators();
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
                unsigned int stage);
        bool completed();
        unsigned int getStage() const;
        void afterTick(int engineTicks) override;
    };
}

#endif
