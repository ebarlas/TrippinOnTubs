#include "Layer.h"

void trippin::Layer::init(SpriteManager &sm, Map::Layer &layer) {
    auto &scale = sm.getScale();
    for (auto &obj : layer.objects) {
        auto &sprite = sm.get(obj.type);
        objects.push_back({scale.scale(obj.position), &sprite});
    }
    size = scale.scale(layer.size);
}

void trippin::Layer::render(const trippin::Camera &camera) {
    auto universe = camera.getUniverse();
    auto viewport = camera.getViewport();
    Point<int> universeRange{universe.w - viewport.w, universe.h - viewport.h};
    Point<int> layerRange{size.x - viewport.w, size.y - viewport.h};
    viewport.x *= static_cast<double>(layerRange.x) / universeRange.x;
    viewport.y *= static_cast<double>(layerRange.y) / universeRange.y;

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
