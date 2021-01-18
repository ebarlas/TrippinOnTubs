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
//    auto xRange = universe.w - viewport.w;
//    auto yRange = universe.h - viewport.h;
    Point<int> layerRange{size.x - viewport.w, size.y - viewport.h};
//    auto xLayerRange = size.x - viewport.w;
//    auto yLayerRange = size.y - viewport.h;
//    double xRatio = static_cast<double>(xLayerRange) / xRange;
//    double yRatio = static_cast<double>(yLayerRange) / yRange;
    viewport.x *= static_cast<double>(layerRange.x) / universeRange.x;
    viewport.y *= static_cast<double>(layerRange.y) / universeRange.y;

    for (auto &obj : objects) {
        auto spriteSize = obj.sprite->getSize();
        Rect<int> box{obj.position.x, obj.position.y, spriteSize.x, spriteSize.y};
        if (box.intersect(viewport)) {
            Point<int> target = {box.x - viewport.x, box.y - viewport.y};
            obj.sprite->render(target, 0);
        }
    }
}
