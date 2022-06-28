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
                SceneBuilder &sceneBuilder);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
    private:
        const Activation &activation;
        const Spirit &spirit;
        const Camera &camera;
        SceneBuilder &sceneBuilder;

        bool melting;
        int meltingTick;
        int frame;
    };
}

#endif
