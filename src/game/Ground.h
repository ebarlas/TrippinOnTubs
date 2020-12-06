#ifndef TRIPPIN_GROUND_H
#define TRIPPIN_GROUND_H

#include "SpriteObject.h"
#include "Spirit.h"

namespace trippin {
    class Ground : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void afterTick(Uint32 engineTicks) override;
        void render(SDL_Renderer *renderer, const Camera &camera) override;
        void setSpirit(const Spirit *spirit);
    private:
        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };

        bool melting;
        int meltingTick;
        int ticks{};
        int framePeriod;
        Channel channel;
        Channel getChannel();
        const Spirit *spirit;
    };
}

#endif
