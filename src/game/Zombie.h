#ifndef TRIPPIN_ZOMBIE_H
#define TRIPPIN_ZOMBIE_H

#include "SpriteObject.h"
#include "lock/Guarded.h"

namespace trippin {
    class Zombie : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
    private:
        int framePeriod;
        double runningAcceleration;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };
        Guarded<Channel> channel;
    };
}

#endif
