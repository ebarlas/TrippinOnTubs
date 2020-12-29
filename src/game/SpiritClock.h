#ifndef TRIPPIN_SPIRITCLOCK_H
#define TRIPPIN_SPIRITCLOCK_H

#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"
#include "lock/Mutex.h"

namespace trippin {
    class SpiritClock : public Renderable, public Listener {
    private:
        const Spirit *spirit{};
        const Sprite *sprite{};
        const Goggin *goggin{};
        Point<int> position;
        double engineTicksPerClockBar;

        Mutex mutex;
        struct Channel {
            int frame;
        };
        Channel channel;
        void setFrame(int frame);
        int getFrame();
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
        void setSpirit(const Spirit *spirit);
        void setGoggin(const Goggin *goggin);
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
    };
}

#endif