#include "Ground.h"

trippin::Ground::Ground(
        const Map::Object &object,
        const Sprite &sprite,
        const Activation &activation,
        const Spirit &spirit,
        const Camera &camera,
        SceneBuilder &sceneBuilder,
        int zIndex,
        const Units &units) :
        SpriteObject(object, sprite, units),
        activation(activation),
        spirit(spirit),
        camera(camera),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex) {
    frame = 0;
    melting = false;
    meltingTick = 0;
    inactive = true;
}

void trippin::Ground::beforeTick(Uint32 engineTicks) {
    if (inactive && activation.shouldActivate(box)) {
        inactive = false;
    }
}

void trippin::Ground::afterTick(Uint32 engineTicks) {
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

    auto posNow = position;
    auto frameNow = frame;
    sceneBuilder.dispatch([this, posNow, frameNow]() {
        sprite.render(posNow, frameNow, camera);
    }, zIndex);
}