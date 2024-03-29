#include "Ground.h"

#include <utility>

trippin::Ground::Ground(
        const Map::Object &object,
        const Sprite &sprite,
        Activation activation,
        const Spirit &spirit,
        const Camera &camera,
        SceneBuilder &sceneBuilder) :
        SpriteObject({}, object, sprite),
        activation(std::move(activation)),
        spirit(spirit),
        camera(camera),
        sceneBuilder(sceneBuilder) {
    platform = true;
    frame = 0;
    melting = false;
    meltingTick = 0;
    inactive = true;
}

void trippin::Ground::beforeTick(int) {
    if (inactive && activation.shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Ground::afterTick(int engineTicks) {
    if (inactive) {
        return;
    }

    if (!melting && position.x <= spirit.getPosition()) {
        melting = true;
        meltingTick = engineTicks;
    }

    if (melting) {
        auto diff = engineTicks - meltingTick;
        if (diff % sprite.getFramePeriodTicks() == 0 && frame < sprite.getFrames()) {
            frame++;
        }
        if (frame == sprite.getFrames() - 1) {
            expired = true;
        }
    }

    auto posNow = roundedPosition;
    auto frameNow = frame;
    auto vp = camera.getViewport();
    sceneBuilder.dispatch([this, posNow, frameNow, vp]() {
        sprite.renderEngine(posNow, frameNow, vp);
    });
}