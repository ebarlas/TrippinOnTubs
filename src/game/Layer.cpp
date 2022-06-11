#include "Layer.h"
#include "engine/Convert.h"

trippin::Layer::Layer(
        const Configuration &config,
        SpriteManager &spriteManager,
        const Map::Layer &layer,
        const Camera &camera,
        SceneBuilder &sceneBuilder,
        int zIndex,
        const Units &units) :
        objects(convertObjects(config, spriteManager, layer)),
        camera(camera),
        size(layer.size),
        anchorTop(layer.anchorTop),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        units(units) {

}

void trippin::Layer::updateStaticObject(const Rect<int_fast64_t> &viewport, const Layer::Object &obj) const {
    auto spriteSize = units.spriteToEngine(obj.sprite->getSize());
    for (int i = 0; i < obj.sprite->getFrames(); i++) {
        Rect<int_fast64_t> box{obj.position.x + i * spriteSize.x, obj.position.y, spriteSize.x, spriteSize.y};
        if (box.intersect(viewport)) {
            auto target = units.engineToSprite(Point<int_fast64_t>{box.x - viewport.x, box.y - viewport.y});
            sceneBuilder.dispatch([obj, target, i]() {
                obj.sprite->render(target, i);
            }, zIndex);
        }
    }
}

void
trippin::Layer::updateAnimatedObject(const Rect<int_fast64_t> &viewport, Layer::Object &obj, Uint32 engineTicks) const {
    obj.x += obj.velocityX;
    if (engineTicks % obj.sprite->getFramePeriodTicks() == 0) {
        obj.frame = (obj.frame + 1) % obj.sprite->getFrames();
    }

    auto target = units.engineToSprite(Point<int_fast64_t>{
            obj.x + obj.position.x - viewport.x,
            obj.position.y - viewport.y});
    auto sp = obj.sprite;
    auto fr = obj.frame;
    sceneBuilder.dispatch([sp, fr, target]() {
        sp->render(target, fr);
    }, zIndex);
}

void trippin::Layer::afterTick(Uint32 engineTicks) {
    auto viewport = makeViewport();

    for (auto &obj: objects) {
        if (obj.animated) {
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
    std::vector<Object> objects;
    for (auto &obj: layer.objects) {
        auto &sprite = spriteManager.get(obj.type);
        auto frame = obj.randFrame ? std::rand() % sprite.getFrames() : 0;
        auto velMs = Fraction<int_fast64_t>{obj.velocity.x, 1000};
        auto vel = velMs * static_cast<Fraction<int_fast64_t>>(config.msPerTick()); // (px per ms) * (ms per tick) = px per tick
        objects.push_back({obj.position, &sprite, obj.animated, frame, 0, static_cast<int_fast64_t>(vel)});
    }
    return objects;
}

trippin::Rect<int_fast64_t> trippin::Layer::makeViewport() {
    auto universe = camera.getUniverse(); // engine scale
    auto viewport = camera.getViewport(); // engine scale
    Point<int_fast64_t> universeRange{universe.w - viewport.w, universe.h - viewport.h};
    Point<int_fast64_t> layerRange{size.x - viewport.w, size.y - viewport.h};
    viewport.x *= toDouble(layerRange.x) / universeRange.x;
    viewport.y *= toDouble(layerRange.y) / universeRange.y;

    // anchor camera to the bottom of the layer if the viewport is taller than the layer
    if (layerRange.y < 0) {
        viewport.y = size.y - viewport.h;
    }

    if (anchorTop) {
        viewport.y = 0;
    }

    return viewport;
}
