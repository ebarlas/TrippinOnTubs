#ifndef TRIPPIN_GROUND_H
#define TRIPPIN_GROUND_H

#include "SpriteObject.h"
#include "Spirit.h"
#include "Activation.h"

namespace trippin {
    class Ground : public SpriteObject {
    public:
        Ground(
                const Map::Object &object,
                const Sprite &sprite,
                const Activation &activation,
                const Spirit &spirit,
                const Camera &camera,
                SceneBuilder &sceneBuilder,
                int zIndex,
                const Units &units);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    private:
        const Activation &activation;
        const Spirit &spirit;
        const Camera &camera;
        SceneBuilder &sceneBuilder;
        int zIndex;

        bool melting;
        Uint32 meltingTick;
        int frame;
    };
}

#endif
