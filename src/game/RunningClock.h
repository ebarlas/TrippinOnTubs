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
                const Configuration &config,
                const Configuration::Object &configObject,
                const Map::Object &object,
                const Sprite &sprite,
                const Sprite &bonusText,
                Goggin &goggin,
                Spirit &spirit,
                Activation activation,
                ScoreTicker &scoreTicker,
                SoundManager &soundManager,
                const Camera &camera,
                SceneBuilder &sceneBuilder,
                NotificationDrawer &notificationDrawer);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
    private:
        constexpr static const int FRAME_CLOUD_FIRST = 24;

        const Configuration &config;
        const Sprite &bonusText;
        Goggin &goggin;
        Spirit &spirit;
        const Activation activation;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        NotificationDrawer &notificationDrawer;
        const Camera &camera;

        const double runningAcceleration;
        Mix_Chunk *const sound;

        int hitTicks;
        bool hitGoggin;
        int frame;
    };
}

#endif
