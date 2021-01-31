#ifndef TRIPPIN_WINGEDTUB_H
#define TRIPPIN_WINGEDTUB_H

#include "engine/Listener.h"
#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"
#include "Score.h"
#include "Activation.h"
#include "lock/Guarded.h"

namespace trippin {
    class WingedTub : public Renderable, public Listener {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
        void setGoggin(const Goggin *goggin);
        void setScore(Score *score);
        void setActivation(const Activation *activation);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        bool isExpired() override;
    private:
        const Sprite *sprite;
        const Goggin *goggin;
        Score *score;
        Point<int> position;
        Rect<int> hitBox;
        int hitTicks;
        bool hitGoggin;
        bool expired;
        bool inactive;
        const Activation *activation;

        struct Channel {
            int frame;
            bool visible;
        };
        Guarded<Channel> channel;
    };
}

#endif
