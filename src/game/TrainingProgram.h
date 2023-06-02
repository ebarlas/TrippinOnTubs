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
        static constexpr int NUM_STAGES = 7;
        static const std::array<const char *, NUM_STAGES> names;
        const std::array<const Sprite *, NUM_STAGES> titleSprites;
        const std::array<const Sprite *, NUM_STAGES> controlSprites;

        const LevelStats &stats;
        const Point<int> windowSize;
        const int margin;
        unsigned int stage;
        Interpolator titleInterpolator;
        Interpolator controlInterpolator;
        const int finishedWaitTicks;
        int stageTicks;
        Mix_Chunk *const sound;

        SceneBuilder &sceneBuilder;

        std::atomic_bool complete;

        void resetInterpolators();

        static std::array<const Sprite *, NUM_STAGES>
        makeSprites(SpriteManager &spriteManager, const std::string &suffix = "");
    public:
        TrainingProgram(
                Point<int> windowSize,
                int margin,
                const Configuration &configuration,
                SpriteManager &spriteManager,
                SoundManager &soundManager,
                const LevelStats &stats,
                const RenderClock &renderClock,
                SceneBuilder &sb);
        bool completed();
        void afterTick(int engineTicks) override;
    };
}

#endif
