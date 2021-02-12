#ifndef TRIPPIN_BIRD_H
#define TRIPPIN_BIRD_H

#include "SpriteObject.h"
#include "Activation.h"
#include "lock/Guarded.h"

namespace trippin {
    class Bird : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        void setActivation(const Activation *activation);
    private:
        const Activation *activation;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };
        Guarded<Channel> channel;
    };
}

#endif
