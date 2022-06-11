#ifndef TRIPPIN_LIFEMETER_H
#define TRIPPIN_LIFEMETER_H

#include "engine/Listener.h"
#include "sprite/Sprite.h"
#include "SceneBuilder.h"

namespace trippin {
    class LifeMeter : public Listener {
    public:
        LifeMeter(
                int margin,
                const Sprite &sprite,
                int extraLives,
                Rect<int> windowSize,
                SceneBuilder &sceneBuilder,
                int zIndex);
        void afterTick(Uint32 engineTicks) override;
    private:
        const int margin; // sprite scale
        const Sprite &sprite;
        const int extraLives;
        const Rect<int> windowSize; // sprite scale
        SceneBuilder &sceneBuilder;
        const int zIndex;
    };
}

#endif
