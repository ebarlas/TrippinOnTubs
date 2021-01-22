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
        };

        std::vector<Object> objects;
        Point<int> size;
        bool anchorTop;
    };
}

#endif
