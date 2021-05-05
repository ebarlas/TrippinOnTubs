#include "Layer.h"
#include "engine/Convert.h"

void trippin::Layer::init(SpriteManager &sm, Map::Layer &layer) {
    for (auto &obj : layer.objects) {
        auto &sprite = sm.get(obj.type);
        int frame = obj.randFrame ? std::rand() % sprite.getFrames() : 0;
        auto vel = obj.velocity.x / 1000.0; // pixels per ms
        objects.push_back({obj.position, &sprite, obj.animated, frame, 0, 0, 0, vel});
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

    auto now = SDL_GetTicks();

    for (auto &obj : objects) {
        if (obj.animated) {
            updateAnimatedObject(viewport, obj, now);
        } else {
            updateStaticObject(viewport, obj);
        }
    }
}

void trippin::Layer::updateStaticObject(const Rect<int> &viewport, const Layer::Object &obj) const {
    auto spriteSize = obj.sprite->getSize();
    for (int i = 0; i < obj.sprite->getFrames(); i++) {
        Rect<int> box{obj.position.x + i * spriteSize.x, obj.position.y, spriteSize.x, spriteSize.y};
        if (box.intersect(viewport)) {
            Point<int> target = {box.x - viewport.x, box.y - viewport.y};
            obj.sprite->render(target, i);
        }
    }
}

void trippin::Layer::updateAnimatedObject(const Rect<int> &viewport, Layer::Object &obj, Uint32 now) const {
    if (obj.lastTime) {
        auto elapsed = now - obj.lastTime;
        obj.x += elapsed * obj.velocityX;
    }
    obj.lastTime = now;

    auto elapsed = now - obj.frameTime;
    if (elapsed >= obj.sprite->getFrameDuration()) {
        obj.frameTime = now;
        obj.frame = (obj.frame + 1) % obj.sprite->getFrames();
    }

    Point<int> target = {(int) obj.x + obj.position.x - viewport.x, obj.position.y - viewport.y};
    obj.sprite->render(target, obj.frame);
}
