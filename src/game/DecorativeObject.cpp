#include "DecorativeObject.h"

trippin::DecorativeObject::DecorativeObject(
        const Map::Object &object,
        const Sprite &sprite,
        Activation activation,
        const Camera &camera,
        SceneBuilder &sceneBuilder) :
        sprite(sprite),
        activation(std::move(activation)),
        sceneBuilder(sceneBuilder),
        camera(camera),
        position(object.position),
        hitBox(sprite.getEngineHitBox() + position),
        expired(false),
        inactive(true) {}

void trippin::DecorativeObject::beforeTick(int) {
    if (inactive && activation.shouldActivate(hitBox)) {
        inactive = false;
    }
}

void trippin::DecorativeObject::afterTick(int engineTicks) {
    if (inactive) {
        return;
    }

    if (activation.shouldDeactivate(hitBox)) {
        expired = true;
        return;
    }

    if (!expired) {
        auto vp = camera.getViewport();
        sceneBuilder.dispatch([this, vp]() {
            sprite.renderEngine(hitBox.corner(), 0, vp);
        });
    }
}

bool trippin::DecorativeObject::isExpired() {
    return expired;
}
