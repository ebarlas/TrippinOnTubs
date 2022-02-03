#ifndef TRIPPIN_TRAININGPROGRAM_H
#define TRIPPIN_TRAININGPROGRAM_H

#include <array>
#include <atomic>
#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "Goggin.h"
#include "ui/MenuLayout.h"

namespace trippin {
    class TrainingProgram : public Listener {
    private:
        enum TrainingStage {
            jump,
            duck,
            stop,
            chargedJump,
            duckJump,
            doubleJump,
            jumpSlamDown,
            finishedWait,
            finished
        };

        static constexpr int NUM_STAGES = 7;
        const std::array<const Sprite*, NUM_STAGES> sprites;

        const Goggin &goggin;
        MenuLayout<1> menuLayout;
        int stage;
        const int finishedWaitTicks;
        Uint32 stageTicks;
        Mix_Chunk *const sound;

        SceneBuilder &sceneBuilder;
        const int zIndex;

        std::atomic_int channel;

        Uint32 getLastEventTime() const;

        static std::array<const Sprite*, NUM_STAGES> makeSprites(SpriteManager &spriteManager);
    public:
        TrainingProgram(
                Point<int> windowSize,
                const Configuration &configuration,
                SpriteManager &spriteManager,
                SoundManager &soundManager,
                const Goggin &goggin,
                const RenderClock &renderClock,
                SceneBuilder &sb,
                int zIndex);
        bool completed();
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif
