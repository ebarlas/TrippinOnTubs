#include "WingedTub.h"

trippin::WingedTub::WingedTub(
        const Configuration &config,
        const Map::Object &object,
        const Sprite &sprite,
        const Activation &activation,
        Goggin &goggin,
        ScoreTicker &scoreTicker,
        SoundManager &soundManager,
        const Camera &camera,
        SceneBuilder &sceneBuilder) :
        sprite(sprite),
        activation(activation),
        goggin(goggin),
        scoreTicker(scoreTicker),
        sceneBuilder(sceneBuilder),
        camera(camera),
        position(object.position),
        hitBox(sprite.getEngineHitBox() + position),
        tubFrameFirst(object.sparkle ? FRAME_SPARKLE_FIRST : FRAME_TUB_FIRST),
        tubFrameLast(object.sparkle ? FRAME_SPARKLE_LAST : FRAME_TUB_LAST),
        points(object.sparkle ? config.sparkleTubBonusPoints : config.tubBonusPoints),
        sound(object.sparkle ? soundManager.getEffect("chime3") : soundManager.getEffect("chime2")) {
    expired = false;
    hitGoggin = false;
    inactive = true;
    frame = tubFrameFirst + Random<>{}.next() % (tubFrameLast - tubFrameFirst);
}

void trippin::WingedTub::beforeTick(int) {
    if (inactive && activation.shouldActivate(hitBox)) {
        inactive = false;
    }
}

void trippin::WingedTub::afterTick(int engineTicks) {
    if (inactive) {
        return;
    }

    if (activation.shouldDeactivate(hitBox)) {
        expired = true;
        return;
    }

    if (!hitGoggin && hitBox.intersect(goggin.roundedBox)) { // Case #1: Goggin contact
        Mix_PlayChannel(-1, sound, 0);
        hitGoggin = true;
        hitTicks = 0;
        frame = FRAME_CLOUD_FIRST;
        scoreTicker.add(points);
        goggin.addPointCloud(points, engineTicks);
    } else if (hitGoggin) { // Case #2: Advance dust cloud
        hitTicks++;
        if (hitTicks % sprite.getFramePeriodTicks() == 0) {
            frame++;
        }
        if (frame == FRAME_CLOUD_LAST) {
            expired = true;
        }
    } else if (engineTicks % sprite.getFramePeriodTicks() == 0) { // Case #3: Advance wings cycle
        frame++;
        if (frame == tubFrameLast) {
            frame = tubFrameFirst;
        }
    }

    if (!expired) {
        auto frameNow = frame;
        auto vp = camera.getViewport();
        sceneBuilder.dispatch([this, frameNow, vp]() {
            sprite.renderEngine(hitBox.corner(), frameNow, vp);
        });
    }
}

bool trippin::WingedTub::isExpired() {
    return expired;
}