#ifndef TRIPPIN_SPIRITCLOCK_H
#define TRIPPIN_SPIRITCLOCK_H

#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"

namespace trippin {
    class SpiritClock : public Renderable, public Listener {
    private:
        const Spirit *spirit{};
        const Sprite *sprite{};
        const Goggin *goggin{};
        Point<int> position;
        double engineTicksPerClockBar;
        double padding{};
        Guarded <int> channel;
    public:
        void init(const Configuration &config, const Sprite &spr);
        void setSpirit(const Spirit &spirit);
        void setGoggin(const Goggin &goggin);
        void setPosition(Point<int> position);
        void setPadding(double padding);
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
    };
}

#endif