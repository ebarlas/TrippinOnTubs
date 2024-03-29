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
        const double engineTicksPerClockBar;
        const int padding;
        SceneBuilder &sceneBuilder;
    public:
        SpiritClock(
                const Configuration &config,
                const Sprite &sprite,
                const Spirit &spirit,
                const Goggin &goggin,
                Point<int> position,
                int padding,
                SceneBuilder &sceneBuilder);
        void afterTick(int engineTicks) override;
        int getNumClockBars() const;
    };
}

#endif