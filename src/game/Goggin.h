#ifndef TRIPPIN_GOGGIN_H
#define TRIPPIN_GOGGIN_H

#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "SDL_mixer.h"
#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "SpriteObject.h"
#include "SoundManager.h"
#include "GogginInput.h"
#include "GogginInputTick.h"
#include "Shake.h"
#include "SceneBuilder.h"
#include "ComboManager.h"
#include "PointCloudManager.h"

namespace trippin {
    class Goggin : public SpriteObject {
    public:
        Goggin(
                const Configuration &config,
                const Map::Object &object,
                const Sprite &sprite,
                const Sprite &dust,
                const Sprite &dustBlast,
                const Sprite &whiteDustBlast,
                ComboManager &comboManager,
                PointCloudManager &pointCloudManager,
                const std::vector<GogginInputTick> *autoPlay,
                const trippin::Point<int> &universe,
                SoundManager &soundManager,
                Camera &camera,
                SceneBuilder &sceneBuilder,
                int zIndex);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        bool rightOfUniverse() const;
        bool belowUniverse() const;
        void onUserInput(const GogginInput &input);
        double getJumpCharge() const;
        void addPointCloud(int points, Uint32 ticks, bool hit = false);
        Uint32 getLastJumpTicks() const;
        Uint32 getLastDuckTicks() const;
        Uint32 getLastChargedJumpTicks() const;
        Uint32 getLastDuckJumpTicks() const;
        Uint32 getLastDoubleJumpTicks() const;
    private:
        struct Dust {
            Point<int> position;
            int frame;
            char ticks;
            bool white;
        };

        struct Frames {
            int frame;
            std::array<Dust, 5> dusts; // circular queue of dust clouds
            Dust blast;
        };

        const Sprite &dust;
        Uint32 dustTicks;
        int nextDustPos;
        const int dustPeriodTicks;

        int consecutiveJumps;

        const Sprite &dustBlast;
        const Sprite &whiteDustBlast;

        double maxFallingVelocity;
        const Point<int> universe;

        constexpr static const int FRAME_LANDING_FIRST = 15;
        constexpr static const int FRAME_FALLING_FIRST = 12;
        constexpr static const int FRAME_FALLING_LAST = 14;
        constexpr static const int FRAME_RUN_AFTER_LAND = 3;
        constexpr static const int FRAME_LAUNCHING_FIRST = 8;
        constexpr static const int FRAME_LAUNCHING_LAST = 11;
        constexpr static const int FRAME_DUCKING = 17;

        constexpr static const int RUNNING_FRAMES = 8;

        constexpr static const double DUCK_HEIGHT = 0.6;

        Frames frames;

        std::mutex mutex;

        GogginInput stagedInput;

        GogginInput input;
        bool rememberDuckStart;
        std::unordered_map<Uint32, GogginInput> autoPlay;
        bool autoPlayEnabled;

        const bool skipLaunch;
        double jumpVelocity;
        const double risingAcceleration;
        const double runningAcceleration;
        const double duckFriction;

        const double minJumpVelocity;
        const double maxJumpVelocity;
        const double maxDuckJumpVelocity;
        const int minJumpChargeTicks;
        const int maxJumpChargeTicks;
        const int jumpGracePeriodTicks;

        const double shakeAmplitude;
        Shake xShake;
        Shake yShake;

        Uint32 jumpTicks{};
        double jumpPercent{};

        std::atomic_bool rightOfUni;
        std::atomic_bool belowUni;

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

        Mix_Chunk *const jumpSound;
        const int jumpSoundTimeoutTicks;
        Uint32 lastJumpSoundTicks;

        PointCloudManager &pointCloudManager;
        ComboManager &comboManager;
        bool grounded;

        SceneBuilder &sceneBuilder;
        Camera &camera;
        int zIndex;

        Uint32 lastJumpTicks;
        Uint32 lastChargedJumpTicks;
        Uint32 lastDuckJumpTicks;
        Uint32 lastDuckTicks;
        Uint32 lastDoubleJumpTicks;

        void onFalling(Uint32 engineTicks);
        void onLanding(Uint32 engineTicks);
        void onRunning(Uint32 engineTicks);
        void onLaunching(Uint32 engineTicks);
        void onRising(Uint32 engineTicks);
        void onDucking(Uint32 engineTicks);

        void resetDustBlast(bool white);

        void shrinkForDuck();
        void growForStand();

        void playJumpSound(Uint32 engineTicks);
        void transferInput(Uint32 engineTicks);

        void handleDuckStart(Uint32 engineTicks);
        void handleDuckEnd();
        void handleJumpCharge(Uint32 engineTicks);
        void handleJumpRelease(Uint32 engineTicks);

        Point<int> centerCamera();
        void drawDust();
        void drawDustBlast();
    };
}

#endif
