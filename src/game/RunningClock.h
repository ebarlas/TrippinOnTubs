#ifndef TRIPPIN_RUNNINGCLOCK_H
#define TRIPPIN_RUNNINGCLOCK_H

#include "SpriteObject.h"
#include "Activation.h"
#include "Spirit.h"
#include "lock/Guarded.h"

namespace trippin {
    class RunningClock : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void setGoggin(const Goggin *goggin);
        void setSpirit(Spirit *spirit);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        void setActivation(const Activation *activation);
        void setUniverse(Point<int> universe);
    private:
        const Goggin *goggin;
        Spirit *spirit;
        int hitTicks;
        bool hitGoggin;

        double runningAcceleration;
        const Activation *activation;
        Rect<int> universe;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
        };
        Guarded<Channel> channel;
    };
}

#endif
