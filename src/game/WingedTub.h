#ifndef TRIPPIN_WINGEDTUB_H
#define TRIPPIN_WINGEDTUB_H

#include "SpriteObject.h"
#include "Spirit.h"
#include "Goggin.h"

namespace trippin {
    class WingedTub : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void setGoggin(const Goggin *goggin);
        void afterTick(Uint32 engineTicks) override;
    protected:
        Point<int> getPosition() override;
        int getFrame() override;
        bool isVisible() override;
    private:
        struct Channel {
            int frame;
        };

        int framePeriod;
        Channel channel;

        int hitTicks;
        bool hitGoggin;
        const Goggin *goggin;
    };
}

#endif
