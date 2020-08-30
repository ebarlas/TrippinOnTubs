#ifndef TRIPPIN_SIMPLEOBJECT_H
#define TRIPPIN_SIMPLEOBJECT_H

#include "SpriteObject.h"

namespace trippin {
    class SimpleObject : public SpriteObject {
    public:
        void afterTick(const Clock &clock) override;
        void render(SDL_Renderer *renderer, const Camera &camera) override;
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
    private:
        struct Channel {
            Point<int> roundedPosition;
        };

        Channel channel;
        Channel getChannel();
    };
}

#endif
