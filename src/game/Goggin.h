#ifndef TRIPPIN_GOGGIN_H
#define TRIPPIN_GOGGIN_H

#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "SpriteObject.h"

namespace trippin {
    class Goggin : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void afterTick(const Clock &clock) override;
        void render(SDL_Renderer *renderer, const Camera &camera) override;
        void center(Camera &camera);
    private:
        struct Channel {
            Point<int> roundedPosition;
            Point<int> roundedCenter;
            int frame;
        };

        constexpr static const int FRAME_LANDING_FIRST = 15;
        constexpr static const int FRAME_FALLING_FIRST = 12;
        constexpr static const int FRAME_FALLING_LAST = 14;
        constexpr static const int FRAME_RUN_AFTER_LAND = 3;
        constexpr static const int FRAME_LAUNCHING_FIRST = 8;
        constexpr static const int FRAME_LAUNCHING_LAST = 11;

        constexpr static const int RUNNING_FRAMES = 8;

        Channel channel;

        double jumpVelocity;
        double activatedAcceleration;

        int framePeriod{};

        enum State {
            running,
            launching,
            rising,
            falling,
            landing
        };

        State state{};
        int ticks{};

        Channel getChannel();
        void onFalling(const Clock &clock);
        void onLanding(const Clock &clock);
        void onRunning(const Clock &clock);
        void onLaunching(const Clock &clock);
        void onRising(const Clock &clock);
    };
}

#endif
