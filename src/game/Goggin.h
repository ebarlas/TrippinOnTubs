#ifndef TRIPPIN_GOGGIN_H
#define TRIPPIN_GOGGIN_H

#include <vector>
#include <unordered_map>
#include "SDL_mixer.h"
#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "SpriteObject.h"
#include "lock/Guarded.h"
#include "SoundManager.h"
#include "GogginInput.h"
#include "GogginInputTick.h"
#include "Shake.h"

namespace trippin {
    class Goggin : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void setUniverse(const Point<int> &universe);
        void setDust(const Sprite &spr);
        void setDustBlast(const Sprite &spr);
        void setDigits(const Sprite &spr);
        void setSoundManager(SoundManager &sm);
        void setAutoPlay(const std::vector<GogginInputTick> &autoPlay);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        // Anchor the camera on Goggin
        // This is the first step of a frame update
        // The position used here ought to be used in the subsequent render call to avoid jitter
        void centerCamera(Camera &camera);
        bool inUniverse() const;
        bool rightOfUniverse() const;
        void onUserInput(const GogginInput &input);
        double getJumpCharge() const;
        bool maxFallingVelocityAbove(double percent);
        void addPointCloud(int points, Uint32 ticks);
    private:
        struct Dust {
            Point<int> position;
            char frame;
            char ticks;
        };

        struct Frames {
            int frame;
            std::array<Dust, 5> dusts; // circular queue of dust clouds
            Dust blast;
        };

        struct PointCloud {
            Point<int> posStart;
            Point<int> posNow;
            Point<int> distance;
            int points{};
            Uint32 ticks;
        };

        // position data that flows from engine thread to render thread
        struct Channel {
            // goggin top-left corner position, pre-normalized for ducking case
            Point<int> position;

            // goggin center point, normalized
            Point<int> center;

            std::array<PointCloud, 8> pointClouds;

            Frames frames;
            bool expired;
        };

        // sound data that flows from engine thread to render thread
        struct SoundChannel {
            bool playJumpSound;
        };

        // goggin top-left corner, saved to ensure jitter/drift
        // accessed exclusively by render/main thread
        Point<int> cameraPosition;

        Point<int> pointCloudDistanceMin;
        Point<int> pointCloudDistanceMax;
        int pointCloudTicks;
        const Sprite *digits;
        std::array<PointCloud, 8> pointClouds; // circular buffer
        int nextPointCloudPos;

        const Sprite *dust;
        Uint32 dustTicks;
        int nextDustPos;
        int dustPeriodTicks;

        const Sprite *dustBlast;

        double maxFallingVelocity;
        Rect<int> universe;

        constexpr static const int FRAME_LANDING_FIRST = 15;
        constexpr static const int FRAME_FALLING_FIRST = 12;
        constexpr static const int FRAME_FALLING_LAST = 14;
        constexpr static const int FRAME_RUN_AFTER_LAND = 3;
        constexpr static const int FRAME_LAUNCHING_FIRST = 8;
        constexpr static const int FRAME_LAUNCHING_LAST = 11;
        constexpr static const int FRAME_DUCKING = 17;

        constexpr static const int RUNNING_FRAMES = 8;

        Frames frames;

        Guarded<Channel> channel;
        Guarded<SoundChannel> soundChannel;
        Guarded<GogginInput> inputChannel;
        void syncChannel();

        GogginInput input;
        bool rememberDuckStart;
        std::unordered_map<Uint32, GogginInput> autoPlay;
        bool autoPlayEnabled;

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

        double shakeAmplitude;
        Shake xShake;
        Shake yShake;

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

        void onFalling(Uint32 engineTicks);
        void onLanding(Uint32 engineTicks);
        void onRunning(Uint32 engineTicks);
        void onLaunching(Uint32 engineTicks);
        void onRising(Uint32 engineTicks);
        void onDucking(Uint32 engineTicks);

        void resetDustBlast();

        void shrinkForDuck();
        void growForStand();

        void enqueueJumpSound(Uint32 engineTicks);
        void transferInput(Uint32 engineTicks);

        void handleDuckStart();
        void handleDuckEnd();
        void handleJumpCharge(Uint32 engineTicks);
        void handleJumpRelease(Uint32 engineTicks);

        static float decelInterpolation(float input);
    };
}

#endif
