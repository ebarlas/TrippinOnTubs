#ifndef TRIPPIN_RUNNINGCLOCK_H
#define TRIPPIN_RUNNINGCLOCK_H

#include "SpriteObject.h"
#include "Activation.h"
#include "ScoreTicker.h"
#include "Spirit.h"

namespace trippin {
    class RunningClock : public SpriteObject {
    public:
        RunningClock(
                const Map::Object &object,
                const Sprite &sprite,
                Goggin &goggin,
                Spirit &spirit,
                const Activation &activation,
                ScoreTicker &scoreTicker,
                SoundManager &soundManager,
                const Camera &camera,
                SceneBuilder &sceneBuilder,
                int zIndex,
                const Units &units);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    private:
        constexpr static const int FRAME_CLOUD_FIRST = 24;

        Goggin &goggin;
        Spirit &spirit;
        const Activation &activation;
        ScoreTicker &scoreTicker;
        const Camera &camera;
        SceneBuilder &sceneBuilder;
        const int zIndex;

        const int_fast64_t runningAcceleration;
        const int points;
        Mix_Chunk *const sound;

        int hitTicks;
        bool hitGoggin;
        int frame;
    };
}

#endif
