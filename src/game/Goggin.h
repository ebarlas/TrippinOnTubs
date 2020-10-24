#ifndef TRIPPIN_GOGGIN_H
#define TRIPPIN_GOGGIN_H

#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "SpriteObject.h"

namespace trippin {
    class Goggin : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(SDL_Renderer *renderer, const Camera &camera) override;
        void center(Camera &camera);
        void onKeyDown();
        void onKeyUp();
    private:
        struct Channel {
            Point<int> roundedPosition;
            Point<int> roundedCenter;
            int frame;
            bool charge;
            bool jump;
        };

        constexpr static const int FRAME_LANDING_FIRST = 15;
        constexpr static const int FRAME_FALLING_FIRST = 12;
        constexpr static const int FRAME_FALLING_LAST = 14;
        constexpr static const int FRAME_RUN_AFTER_LAND = 3;
        constexpr static const int FRAME_LAUNCHING_FIRST = 8;
        constexpr static const int FRAME_LAUNCHING_LAST = 11;

        constexpr static const int RUNNING_FRAMES = 8;

        Channel channel;

        bool skipLaunch;
        double jumpVelocity;
        double risingAcceleration;
        double runningAcceleration;

        int framePeriod;

        double minJumpVelocity;
        double maxJumpVelocity;
        int minJumpChargeTicks;
        int maxJumpChargeTicks;
        int jumpGracePeriodTicks;

        Uint32 chargeTicks{};

        enum State {
            running,
            launching,
            rising,
            falling,
            landing
        };

        State state{};
        int ticks{};
        Uint32 lastRunTick{};

        Channel getChannel();
        double findJumpVelocity(int ticks) const;
        void onFalling(Uint32 engineTicks);
        void onLanding(Uint32 engineTicks);
        void onRunning(Uint32 engineTicks);
        void onLaunching(Uint32 engineTicks);
        void onRising(Uint32 engineTicks);
    };
}

#endif
