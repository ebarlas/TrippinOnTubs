#ifndef TRIPPIN_GAMEOBJECT_H
#define TRIPPIN_GAMEOBJECT_H

#include "engine/ReflectiveCollision.h"
#include "SpriteObject.h"
#include "Activation.h"
#include "lock/Guarded.h"

namespace trippin {
    class GameObject : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        void setActivation(const Activation *activation);
    private:
        double runningAcceleration;
        const Activation *activation;
        int frame;
        bool accelerateWhenGrounded;

        ReflectiveCollision reflectiveCollision;

        int collisionDuration;
        Uint32 collisionTicks;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
            bool visible;
            bool flash;
        };
        int flashCycle;
        Guarded<Channel> channel;
        void syncChannel(Uint32 engineTicks);
        void advanceFrame(Uint32 engineTicks);
    };
}

#endif
