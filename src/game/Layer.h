#ifndef TRIPPIN_LAYER_H
#define TRIPPIN_LAYER_H

#include <vector>
#include "sprite/SpriteManager.h"
#include "Map.h"
#include "Renderable.h"

namespace trippin {
    class Layer : public Renderable {
    public:
        void init(SpriteManager &sm, Map::Layer &layer);
        void render(const Camera &camera) override;
    private:
        struct Object {
            Point<int> position;
            const Sprite *sprite{};
            bool animated{};
            int frame{};
            Uint32 frameTime{};
            Uint32 lastTime{};
            double x{};
            double velocityX{};
        };

        std::vector<Object> objects;
        Point<int> size;
        bool anchorTop;

        void updateAnimatedObject(const Rect<int> &viewport, Object &obj, Uint32 now) const;
        void updateStaticObject(const Rect<int> &viewport, const Object &obj) const;
    };
}

#endif
