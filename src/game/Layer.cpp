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
    double xRatio = static_cast<double>(size.x) / universe.w;
    double yRatio = static_cast<double>(size.y) / universe.h;

    auto viewport = camera.getViewport();
    viewport.x *= xRatio;
    viewport.y *= yRatio;

    if (viewport.y + viewport.h > size.y) {
        viewport.y = size.y - viewport.h;
    }

    for (auto &obj : objects) {
        auto spriteSize = obj.sprite->getSize();
        Rect<int> box{obj.position.x, obj.position.y, spriteSize.x, spriteSize.y};
        if (box.intersect(viewport)) {
            Point<int> target = {box.x - viewport.x, box.y - viewport.y};
            obj.sprite->render(target, 0);
        }
    }
}
