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
                const Configuration::Object &configObject,
                const Map::Object &object,
                const Sprite &sprite,
                Goggin &goggin,
                Spirit &spirit,
                const Activation &activation,
                ScoreTicker &scoreTicker,
                SoundManager &soundManager,
                const Camera &camera,
                SceneBuilder &sceneBuilder);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
    private:
        constexpr static const int FRAME_CLOUD_FIRST = 24;

        Goggin &goggin;
        Spirit &spirit;
        const Activation &activation;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        const Camera &camera;

        const int points;
        const double runningAcceleration;
        Mix_Chunk *const sound;

        int hitTicks;
        bool hitGoggin;
        int frame;
    };
}

#endif
