#ifndef TRIPPIN_GOGGIN_H
#define TRIPPIN_GOGGIN_H

#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "SDL_mixer.h"
#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "sprite/SpriteManager.h"
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
                const Configuration::Object &configObject,
                const Map::Object &object,
                SpriteManager &spriteManager,
                ComboManager &comboManager,
                PointCloudManager &pointCloudManager,
                const std::vector<GogginInputTick> *autoPlay,
                const trippin::Point<int> &universe,
                SoundManager &soundManager,
                Camera &camera,
                SceneBuilder &sceneBuilder);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
        bool rightOfUniverse() const;
        bool belowUniverse() const;
        void onUserInput(const GogginInput &input);
        double getJumpCharge() const;
        void addPointCloud(int points, int engineTicks, bool hit = false);
        int getLastJumpTicks() const;
        int getLastDuckTicks() const;
        int getLastChargedJumpTicks() const;
        int getLastDuckJumpTicks() const;
        int getLastDoubleJumpTicks() const;
        int getLastStopTicks() const;
        int getLastJumpSlamDownTicks() const;
        std::vector<GogginInputTick> takeInputEvents();
        void render();
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
        int dustTicks;
        unsigned int nextDustPos;
        const int dustPeriodTicks;

        int consecutiveJumps;

        const Sprite &dustBlast;
        const Sprite &whiteDustBlast;

        double maxFallingVelocity;
        const Point<int> universe;

        constexpr static const int FRAME_FALLING_FIRST = 12;
        constexpr static const int FRAME_FALLING_LAST = 14;
        constexpr static const int FRAME_RUN_AFTER_LAND = 3;
        constexpr static const int FRAME_LAUNCHING_LAST = 11;
        constexpr static const int FRAME_DUCKING = 17;

        constexpr static const int RUNNING_FRAMES = 8;

        constexpr static const double DUCK_HEIGHT = 0.6;

        Frames frames;

        std::mutex mutex;

        GogginInput stagedInput;

        GogginInput input;
        bool rememberDuckStart;
        std::unordered_map<int, GogginInput> autoPlay;
        bool autoPlayEnabled;

        const double risingAcceleration;
        const double runningAcceleration;
        const double duckFriction;

        const double minJumpVelocity;
        const double maxJumpVelocity;
        const double maxDuckJumpVelocity;
        const int minJumpChargeTicks;
        const int maxJumpChargeTicks;

        const double shakeAmplitude;
        Shake xShake;
        Shake yShake;

        int jumpTicks{};
        double jumpPercent{};

        std::atomic_bool rightOfUni;
        std::atomic_bool belowUni;

        enum State {
            running,
            rising,
            falling,
            ducking
        };

        State state{};
        int ticks{};

        Mix_Chunk *const jumpSound;
        const int jumpSoundTimeoutTicks;
        int lastJumpSoundTicks;

        PointCloudManager &pointCloudManager;
        ComboManager &comboManager;
        bool grounded;

        SceneBuilder &sceneBuilder;
        Camera &camera;

        int lastJumpTicks;
        int lastChargedJumpTicks;
        int lastDuckJumpTicks;
        int lastDuckTicks;
        int lastDoubleJumpTicks;
        int lastStopTicks;
        int lastJumpSlamDownTicks;

        std::vector<GogginInputTick> inputEvents;

        Point<int> cameraPos;

        void onFalling(int engineTicks);
        void onRunning(int engineTicks);
        void onRising(int engineTicks);
        void onDucking(int engineTicks);

        void resetDustBlast(bool white);

        void shrinkForDuck();
        void growForStand();

        void playJumpSound(int engineTicks);
        void transferInput(int engineTicks);

        void handleDuckStart(int engineTicks);
        void handleDuckEnd();
        void handleJumpCharge(int engineTicks);
        void handleJumpRelease(int engineTicks);

        Point<int> centerCamera();
        void drawDust();
        void drawDustBlast();
    };
}

#endif
