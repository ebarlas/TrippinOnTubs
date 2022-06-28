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
                Rect<int> viewport,
                SceneBuilder &sceneBuilder);
        void afterTick(Uint32 engineTicks) override;
    private:
        const Sprite &sprite;
        const int margin;
        const int extraLives;
        const Rect<int> viewport;
        SceneBuilder &sceneBuilder;
    };
}

#endif
