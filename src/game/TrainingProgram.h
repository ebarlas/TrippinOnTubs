#ifndef TRIPPIN_TRAININGPROGRAM_H
#define TRIPPIN_TRAININGPROGRAM_H

#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "Goggin.h"
#include "ui/MenuLayout.h"
#include "lock/Guarded.h"

namespace trippin {
    class TrainingProgram : public Listener {
    private:
        enum TrainingStage {
            jump,
            duck,
            chargedJump,
            duckJump,
            doubleJump,
            finishedWait,
            finished
        };

        static constexpr int NUM_STAGES = 5;
        const Sprite *sprites[NUM_STAGES];

        Goggin &goggin;
        MenuLayout<1> menuLayout;
        int stage;
        int finishedWaitTicks;
        Uint32 stageTicks;
        Mix_Chunk *sound;

        Guarded<int> stageChannel;

        Uint32 getLastEventTime() const;
    public:
        TrainingProgram(
                Point<int> windowSize,
                Configuration &configuration,
                SpriteManager &spriteManager,
                SoundManager &soundManager,
                Goggin &goggin);
        void render();
        bool completed();
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif
