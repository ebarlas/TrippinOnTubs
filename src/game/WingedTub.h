#ifndef TRIPPIN_WINGEDTUB_H
#define TRIPPIN_WINGEDTUB_H

#include "engine/Listener.h"
#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"
#include "Score.h"
#include "lock/Guarded.h"

namespace trippin {
    class WingedTub : public Renderable, public Listener {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
        void setGoggin(const Goggin *goggin);
        void setScore(Score *score);
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        bool isExpired() override;
    private:
        const Sprite *sprite;
        const Goggin *goggin;
        Score *score;
        Point<int> position;
        Rect<int> hitBox;
        int framePeriod;
        int hitTicks;
        bool hitGoggin;
        bool expired;

        struct Channel {
            int frame;
            bool visible;
        };
        Guarded<Channel> channel;
    };
}

#endif
