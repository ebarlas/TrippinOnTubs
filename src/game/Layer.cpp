#include "Layer.h"
#include "Random.h"

trippin::Layer::Layer(
        const Configuration &config,
        SpriteManager &spriteManager,
        const Map::Layer &layer,
        const Camera &camera,
        SceneBuilder &sceneBuilder) :
        camera(camera),
        size(layer.size),
        anchorTop(layer.anchorTop),
        sceneBuilder(sceneBuilder),
        objects(convertObjects(config, spriteManager, layer)) {

}

void trippin::Layer::updateStaticObject(const Rect<int> &viewport, const Layer::Object &obj) const {
    auto spriteSize = obj.sprite->getEngineSize();
    for (int i = 0; i < obj.sprite->getFrames(); i++) {
        Rect<int> box{obj.position.x + i * spriteSize.x, obj.position.y, spriteSize.x, spriteSize.y};
        if (box.intersect(viewport)) {
            Point<int> target = {box.x - viewport.x, box.y - viewport.y};
            target /= obj.sprite->getScale().getDeviceEngineFactor();
            sceneBuilder.dispatch([obj, target, i]() {
                obj.sprite->renderDevice(target, i);
            });
        }
    }
}

void trippin::Layer::updateAnimatedObject(const Rect<int> &viewport, Layer::Object &obj, int engineTicks) const {
    obj.x += obj.velocityX;
    obj.sprite->advanceFrame(engineTicks, obj.frame);

    Point<int> target = {static_cast<int>(obj.x) + obj.position.x - viewport.x, obj.position.y - viewport.y};
    target /= obj.sprite->getScale().getDeviceEngineFactor();
    auto sp = obj.sprite;
    auto fr = obj.frame;
    sceneBuilder.dispatch([sp, fr, target]() {
        sp->renderDevice(target, fr);
    });
}

void trippin::Layer::afterTick(int engineTicks) {
    auto viewport = makeViewport();

    for (auto &obj: objects) {
        if (obj.velocityX != 0) {
            updateAnimatedObject(viewport, obj, engineTicks);
        } else {
            updateStaticObject(viewport, obj);
        }
    }
}

std::vector<trippin::Layer::Object> trippin::Layer::convertObjects(
        const Configuration &config,
        SpriteManager &spriteManager,
        const Map::Layer &layer) {
    Random<> random;
    std::vector<Object> objects;
    for (auto &obj: layer.objects) {
        auto configObject = config.findLayerObject(obj.type);
        auto &sprite = spriteManager.get(obj.type);
        auto frame = configObject != nullptr && configObject->randFrame ? random.next() % sprite.getFrames() : 0;
        auto vel = configObject != nullptr ? configObject->velocity.x : 0;
        objects.push_back({obj.position, &sprite, frame, 0, vel});
    }
    return objects;
}

trippin::Rect<int> trippin::Layer::makeViewport() {
    auto universe = camera.getUniverse();
    auto viewport = camera.getViewport();
    Point<int> universeRange{universe.w - viewport.w, universe.h - viewport.h};
    Point<int> layerRange{size.x - viewport.w, size.y - viewport.h};
    viewport.x = static_cast<int>(viewport.x * static_cast<double>(layerRange.x) / universeRange.x);
    viewport.y = static_cast<int>(viewport.y * static_cast<double>(layerRange.y) / universeRange.y);

    // anchor camera to the bottom of the layer if the viewport is taller than the layer
    if (layerRange.y < 0) {
        viewport.y = size.y - viewport.h;
    }

    if (anchorTop) {
        viewport.y = 0;
    }

    return viewport;
}
