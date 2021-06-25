#ifndef TRIPPIN_LAYER_H
#define TRIPPIN_LAYER_H

#include <vector>
#include "sprite/SpriteManager.h"
#include "Map.h"
#include "Renderable.h"
#include "RenderClock.h"

namespace trippin {
    class Layer : public Renderable {
    public:
        void init(SpriteManager &sm, Map::Layer &layer, RenderClock &renderClock);
        void render(const Camera &camera) override;
    private:
        struct Object {
            Point<int> position;
            const Sprite *sprite{};
            bool animated{};
            int frame{};
            Uint32 frameTime{};
            double x{};
            double velocityX{};
        };

        RenderClock *renderClock;

        std::vector<Object> objects;
        Point<int> size;
        bool anchorTop;

        void updateAnimatedObject(const Rect<int> &viewport, Object &obj) const;
        void updateStaticObject(const Rect<int> &viewport, const Object &obj) const;
    };
}

#endif
