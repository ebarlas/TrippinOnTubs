#include "Layer.h"
#include "engine/Convert.h"

void trippin::Layer::init(SpriteManager &sm, Map::Layer &layer) {
    for (auto &obj : layer.objects) {
        auto &sprite = sm.get(obj.type);
        objects.push_back({obj.position, &sprite});
    }
    size = layer.size;
    anchorTop = layer.anchorTop;
}

void trippin::Layer::render(const trippin::Camera &camera) {
    auto universe = camera.getUniverse();
    auto viewport = camera.getViewport();
    Point<int> universeRange{universe.w - viewport.w, universe.h - viewport.h};
    Point<int> layerRange{size.x - viewport.w, size.y - viewport.h};
    viewport.x *= toDouble(layerRange.x) / universeRange.x;
    viewport.y *= toDouble(layerRange.y) / universeRange.y;

    // anchor camera to the bottom of the layer if the viewport is taller than the layer
    if (layerRange.y < 0) {
        viewport.y = size.y - viewport.h;
    }

    if (anchorTop) {
        viewport.y = 0;
    }

    for (auto &obj : objects) {
        auto spriteSize = obj.sprite->getSize();
        for (int i = 0; i < obj.sprite->getFrames(); i++) {
            Rect<int> box{obj.position.x + i * spriteSize.x, obj.position.y, spriteSize.x, spriteSize.y};
            if (box.intersect(viewport)) {
                Point<int> target = {box.x - viewport.x, box.y - viewport.y};
                obj.sprite->render(target, i);
            }
        }
    }
}
