#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "lock/Exchange.h"

void trippin::Goggin::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);

    skipLaunch = true;
    runningAcceleration = obj.runningAcceleration;
    risingAcceleration = obj.risingAcceleration;
    minJumpVelocity = obj.minJumpVelocity;
    maxJumpVelocity = obj.maxJumpVelocity;
    minJumpChargeTicks = obj.minJumpChargeTime;
    maxJumpChargeTicks = obj.maxJumpChargeTime;
    jumpGracePeriodTicks = obj.jumpGracePeriod;
    duckFriction = obj.duckFriction;
    state = State::falling;

    channel.ref() = {roundedPosition, roundedPosition, FRAME_FALLING_LAST, false, false};
    for (auto &d : channel.ref().dusts) {
        d.frame = dust->getFrames();
    }

    dustPeriodTicks = obj.dustPeriod;
    nextDustPos = 0;
}

void trippin::Goggin::setDust(const Sprite &spr) {
    dust = &spr;
};

void trippin::Goggin::beforeTick(Uint32 engineTicks) {
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();

    if (ch.duckStart) {
        if (state == running && platformCollisions.testBottom()) {
            state = ducking;
            ticks = 0;
            ch.frame = FRAME_DUCKING;
            acceleration.x = 0;
            friction.x = duckFriction;
        }
    }

    if (ch.duckEnd) {
        ch.duckStart = ch.duckEnd = false;
        if (state == ducking) {
            ticks = 0;
            if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
                state = running;
                ch.frame = FRAME_RUN_AFTER_LAND;
                acceleration.x = runningAcceleration;
            } else {
                state = falling;
                ch.frame = FRAME_FALLING_FIRST;
            }
        }
    }

    double jumpVel;
    if (ch.jumpCharge) {
        if (!jumpTicks) {
            jumpTicks = engineTicks;
        }
        auto relTicks = static_cast<int>(engineTicks - jumpTicks);
        auto range = static_cast<double>(maxJumpChargeTicks - minJumpChargeTicks);
        auto boundedTicks = std::max(minJumpChargeTicks, std::min(relTicks, maxJumpChargeTicks));
        jumpPercent = (boundedTicks - minJumpChargeTicks) / range;
        jumpVel = minJumpVelocity + jumpPercent * (maxJumpVelocity - minJumpVelocity);
    } else {
        jumpPercent = 0;
    }

    if (ch.jumpRelease && jumpTicks) {
        ch.jumpCharge = false;
        ch.jumpRelease = false;
        jumpTicks = 0;
        if (state == running || state == landing || state == ducking ||
            (engineTicks > lastRunOrDuckTick && engineTicks - lastRunOrDuckTick < jumpGracePeriodTicks)) {
            if (skipLaunch) {
                state = State::rising;
                ch.frame = FRAME_LAUNCHING_LAST;
                velocity.y = jumpVel;
            } else {
                state = State::launching;
                ch.frame = FRAME_LAUNCHING_FIRST;
                jumpVelocity = jumpVel;
            }
            acceleration.x = risingAcceleration;
            ticks = 0;
        }
    }
}

void trippin::Goggin::afterTick(Uint32 engineTicks) {
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();
    ticks++;

    // advance dust cloud ticks
    for (auto &d : ch.dusts) {
        if (d.frame < dust->getFrames()) {
            d.ticks++;
            if (d.ticks == dust->getFramePeriodTicks()) {
                d.ticks = 0;
                d.frame++; // may go past last frame, denoting inactive
            }
        }
    }

    // test for creation of new dust cloud
    if (platformCollisions.testBottom() && engineTicks - dustTicks >= dustPeriodTicks) {
        dustTicks = engineTicks;
        auto left = roundedPosition.x - dust->getHitBox().w;
        auto top = roundedPosition.y + size.y - dust->getHitBox().h;
        ch.dusts[nextDustPos] = {{left, top}, 0};
        nextDustPos = (nextDustPos + 1) % ch.dusts.size();
    }

    if (state == State::falling) {
        onFalling(engineTicks, ch);
    } else if (state == State::landing) {
        onLanding(engineTicks, ch);
    } else if (state == State::running) {
        onRunning(engineTicks, ch);
    } else if (state == State::launching) {
        onLaunching(engineTicks, ch);
    } else if (state == State::rising) {
        onRising(engineTicks, ch);
    } else if (state == State::ducking) {
        onDucking(engineTicks, ch);
    }

    ch.roundedPosition = roundedPosition;
}

void trippin::Goggin::center(trippin::Camera &camera) {
    // record position here for use in subsequent render call to avoid jitter
    // jitter emerges when an engine tick updates the position *between* center and render calls
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();
    ch.cameraPosition = ch.roundedPosition;
    camera.centerOn(ch.cameraPosition);
}

void trippin::Goggin::onFalling(Uint32 engineTicks, Channel &ch) {
    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        state = State::landing;
        ticks = 0;
        ch.frame = FRAME_LANDING_FIRST;
        return;
    }

    if (ticks == sprite->getFramePeriodTicks()) {
        ticks = 0;
        auto frame = ch.frame;
        if (frame < FRAME_FALLING_LAST) {
            ch.frame = frame + 1;
        }
    }
}

void trippin::Goggin::onLanding(Uint32 engineTicks, Channel &ch) {
    if (ticks != sprite->getFramePeriodTicks()) {
        return;
    }

    ticks = 0;
    auto frame = ch.frame;
    if (frame == FRAME_LANDING_FIRST) {
        ch.frame = frame + 1;
        return;
    }

    state = running;
    ch.frame = FRAME_RUN_AFTER_LAND;
    acceleration.x = runningAcceleration;
}

void trippin::Goggin::onRunning(Uint32 engineTicks, Channel &ch) {
    lastRunOrDuckTick = engineTicks;

    if (!platformCollisions.testBottom() && !objectCollisions.testBottom()) {
        state = State::falling;
        ch.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        return;
    }

    if (ticks == sprite->getFramePeriodTicks()) {
        ticks = 0;
        ch.frame = (ch.frame + 1) % RUNNING_FRAMES;
    }
}

void trippin::Goggin::onLaunching(Uint32 engineTicks, Channel &ch) {
    if (ticks != sprite->getFramePeriodTicks()) {
        return;
    }

    ticks = 0;
    auto frame = ch.frame;
    if (frame < FRAME_LAUNCHING_LAST) {
        frame++;
        ch.frame = frame;
        if (frame == FRAME_LAUNCHING_LAST) {
            state = State::rising;
            velocity.y = jumpVelocity;
        }
    }
}

void trippin::Goggin::onRising(Uint32 engineTicks, Channel &ch) {
    if (velocity.y >= 0) {
        state = State::falling;
        ch.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
    }
}

void trippin::Goggin::onDucking(Uint32 engineTicks, Channel &ch) {
    lastRunOrDuckTick = engineTicks;

    if (!platformCollisions.testBottom()) {
        ticks = 0;
        if (objectCollisions.testBottom()) {
            state = running;
            ch.frame = FRAME_RUN_AFTER_LAND;
            acceleration.x = runningAcceleration;
        } else {
            state = falling;
            ch.frame = FRAME_FALLING_FIRST;
        }
    }
}

void trippin::Goggin::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.cameraPosition, ch.frame, camera);

    for (auto &d : ch.dusts) {
        if (d.frame < dust->getFrames()) {
            dust->render(d.position, d.frame, camera);
        }
    }
}

void trippin::Goggin::onJumpCharge() {
    Exchange<Channel> exchange{channel};
    exchange.get().jumpCharge = true;
}

void trippin::Goggin::onJumpRelease() {
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();
    if (ch.jumpCharge) {
        ch.jumpRelease = true;
    }
}

double trippin::Goggin::getJumpCharge() const {
    return jumpPercent;
}

void trippin::Goggin::onDuckStart() {
    Exchange<Channel> exchange{channel};
    exchange.get().duckStart = true;
}

void trippin::Goggin::onDuckEnd() {
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();
    if (ch.duckStart) {
        ch.duckEnd = true;
    }
}
