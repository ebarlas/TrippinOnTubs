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
    public:
        JumpMeter(
                const Sprite &sprite,
                const Goggin &goggin,
                Point<int> position,
                SceneBuilder &sceneBuilder);
        void afterTick(int engineTicks) override;
    };
}

#endif