#include "RunningClock.h"

void trippin::RunningClock::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    lane = -1;
    inactive = true;
    runningAcceleration = obj.runningAcceleration;
    points = obj.points;
    frame = 0;
    playedSound = false;
    sound = soundManager->getEffect("chime1");
    syncChannel();
}

void trippin::RunningClock::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::RunningClock::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        syncChannel();
        return;
    }

    // Case #1: Goggin contact
    if (!hitGoggin && roundedBox.intersect(goggin->roundedBox)) {
        hitGoggin = true;
        hitTicks = 0;
        frame = FRAME_CLOUD_FIRST;
        spirit->delay(1);
        scoreTicker->add(points);
        goggin->addPointCloud(points, engineTicks);
    }

    // Case #2: Advance dust cloud
    else if (hitGoggin) {
        hitTicks++;
        if (hitTicks % (sprite->getFramePeriodTicks() * 2) == 0) {
            frame++;
        }
        if (frame == sprite->getFrames()) {
            expired = true;
        }
    }

    // Case #3: Advance flapping wings cycle
    else if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        acceleration.x = runningAcceleration;
        if (engineTicks % sprite->getFramePeriodTicks() == 0) {
            frame = (frame + 1) % FRAME_CLOUD_FIRST;
        }
    } else {
        acceleration.x = 0;
    }

    syncChannel();
}

void trippin::RunningClock::setGoggin(Goggin *g) {
    goggin = g;
}

void trippin::RunningClock::setSpirit(Spirit *sp) {
    spirit = sp;
}

void trippin::RunningClock::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (ch.visible) {
        sprite->render(ch.roundedPosition, ch.frame, camera);
        if (ch.frame == FRAME_CLOUD_FIRST && !playedSound) {
            Mix_PlayChannel( -1, sound, 0 );
            playedSound = true;
        }
    }
}

void trippin::RunningClock::setActivation(const Activation *act) {
    activation = act;
}

void trippin::RunningClock::setScoreTicker(trippin::ScoreTicker *score) {
    scoreTicker = score;
}

void trippin::RunningClock::setSoundManager(trippin::SoundManager &sm) {
    soundManager = &sm;
}

void trippin::RunningClock::syncChannel() {
    channel.set({roundedPosition, frame, !inactive && !expired});
}
