#ifndef TRIPPIN_TRAININGPROGRAM_H
#define TRIPPIN_TRAININGPROGRAM_H

#include <array>
#include <atomic>
#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "Goggin.h"
#include "ui/Interpolator.h"

namespace trippin {
    class TrainingProgram : public Listener {
    private:
        static constexpr int NUM_STAGES = 7;
        static const std::array<const char *, NUM_STAGES> names;
        const std::array<std::function<Uint32(const Goggin &g)>, NUM_STAGES> lastEventTimes{
                [](const Goggin &g) { return g.getLastJumpTicks(); },
                [](const Goggin &g) { return g.getLastDuckTicks(); },
                [](const Goggin &g) { return g.getLastStopTicks(); },
                [](const Goggin &g) { return g.getLastChargedJumpTicks(); },
                [](const Goggin &g) { return g.getLastDuckJumpTicks(); },
                [](const Goggin &g) { return g.getLastDoubleJumpTicks(); },
                [](const Goggin &g) { return g.getLastJumpSlamDownTicks(); }
        };
        const std::array<const Sprite *, NUM_STAGES> titleSprites;
        const std::array<const Sprite *, NUM_STAGES> controlSprites;

        const Goggin &goggin;
        const Point<int> windowSize;
        const int margin;
        int stage;
        Interpolator titleInterpolator;
        Interpolator controlInterpolator;
        const int finishedWaitTicks;
        Uint32 stageTicks;
        Mix_Chunk *const sound;

        SceneBuilder &sceneBuilder;
        const int zIndex;

        std::atomic_bool complete;

        void resetInterpolators();

        static std::array<const Sprite *, NUM_STAGES>
        makeSprites(SpriteManager &spriteManager, const std::string &suffix = "");
    public:
        TrainingProgram(
                Point<int> windowSize, // sprite scale
                int margin, // sprite scale
                const Configuration &configuration,
                SpriteManager &spriteManager,
                SoundManager &soundManager,
                const Goggin &goggin,
                const RenderClock &renderClock,
                SceneBuilder &sb,
                int zIndex);
        bool completed();
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif
