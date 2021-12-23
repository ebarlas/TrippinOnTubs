#ifndef TRIPPIN_COMBOMANAGER_H
#define TRIPPIN_COMBOMANAGER_H

#include <array>
#include "engine/Point.h"
#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "SceneBuilder.h"
#include "ScoreTicker.h"

namespace trippin {
    class ComboManager : public Listener {
    public:
        ComboManager(
                SpriteManager &spriteManager,
                ScoreTicker &scoreTicker,
                double msPerTick,
                Point<int> windowSize,
                int margin,
                SceneBuilder &sceneBuilder,
                int zIndex);
        void reset();
        void recordHit();
        void afterTick(Uint32 engineTicks) override;
    private:
        struct Display {
            int points{};
            int startTicks{};
        };

        static const int DURATION_SLIDE = 1'000;
        static const int DURATION_PAUSE = 2'000;

        ScoreTicker &scoreTicker;
        const double msPerTick;
        const Point<int> windowSize;
        const int margin;
        const Sprite &comboText;
        const Sprite &digits;
        SceneBuilder &sceneBuilder;
        const int zIndex;

        std::array<Display, 3> displays{};
        int hits{};
        int nextDisplayPos{};

        void render(Uint32 engineTicks, Display &display);

        static double decelInterpolation(float input);
    };
}

#endif