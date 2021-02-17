#include "WingedTub.h"
#include "lock/Exchange.h"
#include "engine/Convert.h"

void trippin::WingedTub::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    position = toInt(obj.position);
    sprite = &spr;
    hitBox = spr.getHitBox() + position;
    expired = false;
    hitGoggin = false;
    inactive = true;
    tubFrameFirst = obj.sparkle ? FRAME_SPARKLE_FIRST : FRAME_TUB_FIRST;
    tubFrameLast = obj.sparkle ? FRAME_SPARKLE_LAST : FRAME_TUB_LAST;
    channel.ref() = {tubFrameFirst, true};
    points = obj.sparkle ? 200 : 100;
    playedSound = false;
    sound = obj.sparkle ? soundManager->getEffect("chime3") : soundManager->getEffect("chime2");
}

void trippin::WingedTub::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(hitBox)) {
        inactive = false;
    }
}

void trippin::WingedTub::afterTick(Uint32 engineTicks) {
    Exchange<Channel> exchange(channel);
    auto &ch = exchange.get();

    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(hitBox)) {
        expired = true;
        return;
    }

    // Case #1: Goggin contact
    if (!hitGoggin && hitBox.intersect(goggin->roundedBox)) {
        hitGoggin = true;
        hitTicks = 0;
        ch.frame = FRAME_CLOUD_FIRST;
        score->add(points);
        return;
    }

    // Case #2: Advance dust cloud
    if (hitGoggin) {
        hitTicks++;
        if (hitTicks % sprite->getFramePeriodTicks() == 0) {
            ch.frame++;
        }
        if (ch.frame == FRAME_CLOUD_LAST) {
            expired = true;
            ch.visible = false;
        }
        return;
    }

    // Case #3: Advance flapping wings cycle
    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        ch.frame++;
        if (ch.frame == tubFrameLast) {
            ch.frame = tubFrameFirst;
        }
    }
}

void trippin::WingedTub::setGoggin(const Goggin *g) {
    goggin = g;
}

void trippin::WingedTub::setScore(Score *sc) {
    score = sc;
}

void trippin::WingedTub::setActivation(const Activation *act) {
    activation = act;
}

void trippin::WingedTub::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (ch.visible) {
        sprite->render(hitBox.corner(), ch.frame, camera);
        if (ch.frame == FRAME_CLOUD_FIRST && !playedSound) {
            Mix_PlayChannel( -1, sound, 0 );
            playedSound = true;
        }
    }
}

bool trippin::WingedTub::isExpired() {
    return expired;
}

void trippin::WingedTub::setSoundManager(trippin::SoundManager &sm) {
    soundManager = &sm;
}
