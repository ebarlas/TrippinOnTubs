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
                int minComboHits,
                double msPerTick,
                Point<int> windowSize,
                int margin,
                SceneBuilder &sceneBuilder);
        int reset();
        void recordHit();
        void afterTick(int engineTicks) override;
    private:
        struct Display {
            int points{};
            int startTicks{};
        };

        static const int DURATION_SLIDE = 1'000;
        static const int DURATION_PAUSE = 2'000;

        const int minComboHits;
        const double msPerTick;
        const Point<int> windowSize;
        const int margin;
        const Sprite &comboText;
        const Sprite &digits;
        SceneBuilder &sceneBuilder;

        std::array<Display, 3> displays{};
        int hits{};
        unsigned int nextDisplayPos{};

        void render(int engineTicks, Display &display);

        static double decelInterpolation(double input);
    };
}

#endif