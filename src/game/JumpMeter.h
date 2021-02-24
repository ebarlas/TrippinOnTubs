#ifndef TRIPPIN_JUMPMETER_H
#define TRIPPIN_JUMPMETER_H

#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"
#include "lock/Guarded.h"

namespace trippin {
    class JumpMeter : public Renderable, public Listener {
    private:
        const Sprite *sprite{};
        const Goggin *goggin{};
        Point<int> position;

        struct Channel {
            int frame;
        };
        Guarded<Channel> gChannel;
    public:
        void init();
        void setPosition(Point<int> position);
        void setSprite(const Sprite &sprite);
        void setGoggin(const Goggin &goggin);
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
    };
}

#endif