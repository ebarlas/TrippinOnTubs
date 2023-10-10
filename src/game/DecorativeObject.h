#ifndef TRIPPIN_DECORATIVEOBJECT_H
#define TRIPPIN_DECORATIVEOBJECT_H

#include "engine/Listener.h"
#include "Activation.h"

namespace trippin {
    class DecorativeObject : public Listener {
    public:
        DecorativeObject(
                const Map::Object &object,
                const Sprite &sprite,
                Activation activation,
                const Camera &camera,
                SceneBuilder &sceneBuilder);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
        bool isExpired() override;
    private:
        const Sprite &sprite;
        const Activation activation;
        SceneBuilder &sceneBuilder;
        const Camera &camera;

        const Point<int> position;
        const Rect<int> hitBox;

        bool expired;
        bool inactive;
    };
}

#endif
