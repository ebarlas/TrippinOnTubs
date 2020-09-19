#ifndef TRIPPIN_GROUND_H
#define TRIPPIN_GROUND_H

#include "SpriteObject.h"

namespace trippin {
    class Ground : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void afterTick(const Clock &clock) override;
        void render(SDL_Renderer *renderer, const Camera &camera) override;
    private:
        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };

        int activationTick{};
        int ticks{};
        int framePeriod;
        Channel channel;
        Channel getChannel();
    };
}

#endif
