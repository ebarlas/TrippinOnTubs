#ifndef TRIPPIN_SPIRITCLOCK_H
#define TRIPPIN_SPIRITCLOCK_H

#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"

namespace trippin {
    class SpiritClock : public Listener {
    private:
        const Spirit &spirit;
        const Sprite &sprite;
        const Goggin &goggin;
        const Point<int> position;
        const Fraction<int> engineTicksPerClockBar;
        const int_fast64_t padding;
        SceneBuilder &sceneBuilder;
        const int zIndex;
    public:
        SpiritClock(
                const Configuration &config,
                const Sprite &sprite,
                const Spirit &spirit,
                const Goggin &goggin,
                Point<int> position, // sprite units
                int_fast64_t padding, // engine unit
                SceneBuilder &sceneBuilder,
                int zIndex);
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif