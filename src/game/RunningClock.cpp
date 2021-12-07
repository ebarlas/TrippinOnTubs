#include "RunningClock.h"

trippin::RunningClock::RunningClock(
        const Configuration &config,
        const Map::Object &object,
        const Sprite &sprite,
        Goggin &goggin,
        Spirit &spirit,
        const Activation &activation,
        ScoreTicker &scoreTicker,
        SoundManager &soundManager,
        const Camera &camera,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        SpriteObject(config, object, sprite),
        goggin(goggin),
        activation(activation),
        spirit(spirit),
        scoreTicker(scoreTicker),
        camera(camera),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        runningAcceleration(object.runningAcceleration),
        points(50),
        sound(soundManager.getEffect("chime1")) {
    lane = -1;
    inactive = true;
    hitGoggin = false;
    hitTicks = 0;
    frame = 0;
}

void trippin::RunningClock::beforeTick(Uint32 engineTicks) {
    if (inactive && activation.shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::RunningClock::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation.shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }


    if (!hitGoggin && roundedBox.intersect(goggin.roundedBox)) { // Case #1: Goggin contact
        Mix_PlayChannel(-1, sound, 0);
        hitGoggin = true;
        hitTicks = 0;
        frame = FRAME_CLOUD_FIRST;
        spirit.delay(1);
        scoreTicker.add(points);
        goggin.addPointCloud(points, engineTicks);
    } else if (hitGoggin) { // Case #2: Advance dust cloud
        hitTicks++;
        if (hitTicks % (sprite.getFramePeriodTicks() * 2) == 0) {
            frame++;
        }
        if (frame == sprite.getFrames()) {
            expired = true;
        }
    } else if (platformCollisions.testBottom() || objectCollisions.testBottom()) { // Case #3: Advance wings cycle
        acceleration.x = runningAcceleration;
        if (engineTicks % sprite.getFramePeriodTicks() == 0) {
            frame = (frame + 1) % FRAME_CLOUD_FIRST;
        }
    } else {
        acceleration.x = 0;
    }

    if (!expired) {
        auto frameNow = frame;
        auto posNow = roundedPosition;
        sceneBuilder.dispatch([this, posNow, frameNow]() {
            sprite.render(posNow, frameNow, camera);
        }, zIndex);
    }
}
