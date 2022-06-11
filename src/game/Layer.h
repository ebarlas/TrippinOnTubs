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
                int zIndex,
                const Units &units);
        void afterTick(Uint32 engineTicks) override;
    private:
        struct Object {
            Point<int_fast64_t> position;
            const Sprite *sprite{};
            bool animated{};
            int frame{};
            int_fast64_t x{};
            int_fast64_t velocityX{};
        };

        const Camera &camera;
        const Point<int_fast64_t> size;
        const bool anchorTop;
        SceneBuilder &sceneBuilder;
        const int zIndex;
        const Units &units;

        std::vector<Object> objects;

        void updateAnimatedObject(const Rect<int_fast64_t> &viewport, Object &obj, Uint32 engineTicks) const;
        void updateStaticObject(const Rect<int_fast64_t> &viewport, const Object &obj) const;

        static std::vector<Object> convertObjects(const Configuration &config, SpriteManager &spriteManager, const Map::Layer &layer);
        Rect<int_fast64_t> makeViewport();
    };
}

#endif
