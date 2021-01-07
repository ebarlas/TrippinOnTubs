#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "lock/Exchange.h"

void trippin::Goggin::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);

    auto mul = spr.getScale().getMultiplier();
    auto gameTicksPerSecondSq = config.ticksPerSecond() * config.ticksPerSecond();

    skipLaunch = true;
    framePeriod = sprite->getDuration() / config.tickPeriod;
    runningAcceleration = (obj.runningAcceleration / gameTicksPerSecondSq) * mul;
    risingAcceleration = (obj.risingAcceleration / gameTicksPerSecondSq) * mul;
    minJumpVelocity = (obj.minJumpVelocity / config.ticksPerSecond()) * mul;
    maxJumpVelocity = (obj.maxJumpVelocity / config.ticksPerSecond()) * mul;
    minJumpChargeTicks = obj.minJumpChargeTime / config.tickPeriod;
    maxJumpChargeTicks = obj.maxJumpChargeTime / config.tickPeriod;
    jumpGracePeriodTicks = obj.jumpGracePeriod / config.tickPeriod;
    state = State::falling;
    channel.ref() = {roundedPosition, 14, false, false};
}

void trippin::Goggin::beforeTick(Uint32 engineTicks) {
    Exchange exchange{channel};
    auto &ch = exchange.get();

    double jumpVel;
    if (ch.keyDown) {
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

    if (ch.keyUp && jumpTicks) {
        ch.keyDown = false;
        ch.keyUp = false;
        jumpTicks = 0;
        if (state == running || state == landing ||
            (engineTicks > lastRunTick && engineTicks - lastRunTick < jumpGracePeriodTicks)) {
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
    Exchange exchange{channel};
    auto &ch = exchange.get();
    ticks++;

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
    }

    ch.roundedPosition = roundedPosition;
}

void trippin::Goggin::center(trippin::Camera &camera) {
    camera.centerOn(channel.get().roundedPosition);
}

void trippin::Goggin::onFalling(Uint32 engineTicks, Channel &ch) {
    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        state = State::landing;
        ticks = 0;
        ch.frame = FRAME_LANDING_FIRST;
        return;
    }

    if (ticks == framePeriod) {
        ticks = 0;
        auto frame = ch.frame;
        if (frame < FRAME_FALLING_LAST) {
            ch.frame = frame + 1;
        }
    }
}

void trippin::Goggin::onLanding(Uint32 engineTicks, Channel &ch) {
    if (ticks != framePeriod) {
        return;
    }

    ticks = 0;
    auto frame = ch.frame;
    if (frame == FRAME_LANDING_FIRST) {
        ch.frame = frame + 1;
        return;
    }

    state = State::running;
    ch.frame = FRAME_RUN_AFTER_LAND;
    acceleration.x = runningAcceleration;
}

void trippin::Goggin::onRunning(Uint32 engineTicks, Channel &ch) {
    lastRunTick = engineTicks;

    if (!platformCollisions.testBottom() && !objectCollisions.testBottom()) {
        state = State::falling;
        ch.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        return;
    }

    if (ticks == framePeriod) {
        ticks = 0;
        ch.frame = (ch.frame + 1) % RUNNING_FRAMES;
    }
}

void trippin::Goggin::onLaunching(Uint32 engineTicks, Channel &ch) {
    if (ticks != framePeriod) {
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

void trippin::Goggin::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Goggin::onKeyDown() {
    Exchange exchange{channel};
    auto &ch = exchange.get();
    ch.keyDown = true;
}

void trippin::Goggin::onKeyUp() {
    Exchange exchange{channel};
    auto &ch = exchange.get();
    if (ch.keyDown) {
        ch.keyUp = true;
    }
}

double trippin::Goggin::getJumpCharge() const {
    return jumpPercent;
}