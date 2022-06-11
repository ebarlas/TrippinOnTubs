#ifndef TRIPPIN_JUMPMETER_H
#define TRIPPIN_JUMPMETER_H

#include "SpriteObject.h"
#include "Goggin.h"

namespace trippin {
    class JumpMeter : public Listener {
    private:
        const Sprite &sprite;
        const Goggin &goggin;
        const Point<int> position;
        SceneBuilder &sceneBuilder;
        const int zIndex;
    public:
        JumpMeter(
                const Sprite &sprite,
                const Goggin &goggin,
                Point<int> position, // sprite scale
                SceneBuilder &sceneBuilder,
                int zIndex);
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif