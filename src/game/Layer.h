#ifndef TRIPPIN_LAYER_H
#define TRIPPIN_LAYER_H

#include <vector>
#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "Map.h"
#include "Configuration.h"
#include "SceneBuilder.h"

namespace trippin {
    class Layer : public Listener {
    public:
        Layer(
                const Configuration &config,
                SpriteManager &spriteManager,
                const Map::Layer &layer,
                const Camera &camera,
                SceneBuilder &sceneBuilder,
                int zIndex);
        void afterTick(Uint32 engineTicks) override;
    private:
        struct Object {
            Point<int> position;
            const Sprite *sprite{};
            int frame{};
            double x{};
            double velocityX{};
        };

        const Camera &camera;
        const Point<int> size;
        const bool anchorTop;
        SceneBuilder &sceneBuilder;
        const int zIndex;

        std::vector<Object> objects;

        void updateAnimatedObject(const Rect<int> &viewport, Object &obj, Uint32 engineTicks) const;
        void updateStaticObject(const Rect<int> &viewport, const Object &obj) const;

        static std::vector<Object> convertObjects(const Configuration &config, SpriteManager &spriteManager, const Map::Layer &layer);
        Rect<int> makeViewport();
    };
}

#endif
