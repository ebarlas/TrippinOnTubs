#ifndef TRIPPIN_GOGGIN_H
#define TRIPPIN_GOGGIN_H

#include <SDL_mixer.h>
#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "SpriteObject.h"
#include "lock/Guarded.h"
#include "SoundManager.h"

namespace trippin {
    class Goggin : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void setDust(const Sprite &spr);
        void setDustBlast(const Sprite &spr);
        void setSoundManager(SoundManager &sm);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        // Anchor the camera on Goggin
        // This is the first step of a frame update
        // The position used here ought to be used in the subsequent render call to avoid jitter
        void centerCamera(Camera &camera);
        bool inUniverse(const Rect<int> &universe) const;
        void onJumpCharge();
        void onJumpRelease();
        void onDuckStart();
        void onDuckEnd();
        double getJumpCharge() const;
    private:
        struct Dust {
            Point<int> position;
            char frame;
            char ticks;
        };

        struct Channel {
            // goggin top-left corner position, pre-normalized for ducking case
            Point<int> position;

            // goggin center point, normalized
            Point<int> center;

            int frame;
            bool jumpCharge;
            bool jumpRelease;
            bool duckStart;
            bool duckEnd;
            std::array<Dust, 5> dusts; // circular queue of dust clouds
            Dust blast;
        };

        struct SoundChannel {
            bool playJumpSound;
        };

        // goggin top-left corner, saved to ensure jitter/drift
        // accessed exclusively by render/main thread
        Point<int> cameraPosition;

        const Sprite *dust;
        Uint32 dustTicks;
        int nextDustPos;
        int dustPeriodTicks;

        const Sprite *dustBlast;

        double maxFallingVelocity;

        constexpr static const int FRAME_LANDING_FIRST = 15;
        constexpr static const int FRAME_FALLING_FIRST = 12;
        constexpr static const int FRAME_FALLING_LAST = 14;
        constexpr static const int FRAME_RUN_AFTER_LAND = 3;
        constexpr static const int FRAME_LAUNCHING_FIRST = 8;
        constexpr static const int FRAME_LAUNCHING_LAST = 11;
        constexpr static const int FRAME_DUCKING = 17;

        constexpr static const int RUNNING_FRAMES = 8;

        Guarded<Channel> channel;
        Guarded<SoundChannel> soundChannel;

        bool skipLaunch;
        double jumpVelocity;
        double risingAcceleration;
        double runningAcceleration;
        double duckFriction;

        double minJumpVelocity;
        double maxJumpVelocity;
        double maxDuckJumpVelocity;
        int minJumpChargeTicks;
        int maxJumpChargeTicks;
        int jumpGracePeriodTicks;

        Uint32 jumpTicks{};
        double jumpPercent{};

        enum State {
            running,
            launching,
            rising,
            falling,
            landing,
            ducking
        };

        State state{};
        int ticks{};
        Uint32 lastRunOrDuckTick{};

        SoundManager *soundManager;
        Mix_Chunk *jumpSound;
        int jumpSoundTimeoutTicks;
        Uint32 lastJumpTicks;

        void onFalling(Uint32 engineTicks, Channel &ch);
        void onLanding(Uint32 engineTicks, Channel &ch);
        void onRunning(Uint32 engineTicks, Channel &ch);
        void onLaunching(Uint32 engineTicks, Channel &ch);
        void onRising(Uint32 engineTicks, Channel &ch);
        void onDucking(Uint32 engineTicks, Channel &ch);

        void resetDustBlast(Channel &ch);

        void shrinkForDuck();
        void growForStand();
        void savePosition(Channel &ch);

        void enqueueJumpSound(Uint32 engineTicks);
    };
}

#endif
