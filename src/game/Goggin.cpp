#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "Lock.h"

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
    channel.frame = 14;
    channel.charge = false;
}

void trippin::Goggin::beforeTick(Uint32 engineTicks) {
    Lock lock(mutex);
    if (channel.charge && !chargeTicks) {
        chargeTicks = engineTicks;
    }
    if (channel.jump && chargeTicks) {
        int jumpTicks = engineTicks - chargeTicks;
        channel.charge = false;
        channel.jump = false;
        chargeTicks = 0;
        if (state == running || (engineTicks > lastRunTick && engineTicks - lastRunTick < jumpGracePeriodTicks)) {
            if (skipLaunch) {
                state = State::rising;
                channel.frame = FRAME_LAUNCHING_LAST;
                velocity.y = findJumpVelocity(jumpTicks);
            } else {
                state = State::launching;
                channel.frame = FRAME_LAUNCHING_FIRST;
                jumpVelocity = findJumpVelocity(jumpTicks);
            }
            acceleration.x = risingAcceleration;
            ticks = 0;
        }
    }
}

void trippin::Goggin::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    ticks++;

    if (state == State::falling) {
        onFalling(engineTicks);
    } else if (state == State::landing) {
        onLanding(engineTicks);
    } else if (state == State::running) {
        onRunning(engineTicks);
    } else if (state == State::launching) {
        onLaunching(engineTicks);
    } else if (state == State::rising) {
        onRising(engineTicks);
    }

    channel.roundedPosition = roundedPosition;
    channel.roundedCenter = roundedCenter;
}

void trippin::Goggin::center(trippin::Camera &camera) {
    camera.centerOn(getPosition());
}

void trippin::Goggin::onFalling(Uint32 engineTicks) {
    if (platformCollisions.testBottom()) {
        state = State::landing;
        ticks = 0;
        channel.frame = FRAME_LANDING_FIRST;
        return;
    }

    if (ticks == framePeriod) {
        ticks = 0;
        auto frame = channel.frame;
        if (frame < FRAME_FALLING_LAST) {
            channel.frame = frame + 1;
        }
    }
}

void trippin::Goggin::onLanding(Uint32 engineTicks) {
    if (ticks != framePeriod) {
        return;
    }

    ticks = 0;
    auto frame = channel.frame;
    if (frame == FRAME_LANDING_FIRST) {
        channel.frame = frame + 1;
        return;
    }

    state = State::running;
    channel.frame = FRAME_RUN_AFTER_LAND;
    acceleration.x = runningAcceleration;
}

void trippin::Goggin::onRunning(Uint32 engineTicks) {
    lastRunTick = engineTicks;

    if (!platformCollisions.testBottom()) {
        state = State::falling;
        channel.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        return;
    }

    if (ticks == framePeriod) {
        ticks = 0;
        channel.frame = (channel.frame + 1) % RUNNING_FRAMES;
    }
}

void trippin::Goggin::onLaunching(Uint32 engineTicks) {
    if (ticks != framePeriod) {
        return;
    }

    ticks = 0;
    auto frame = channel.frame;
    if (frame < FRAME_LAUNCHING_LAST) {
        frame++;
        channel.frame = frame;
        if (frame == FRAME_LAUNCHING_LAST) {
            state = State::rising;
            velocity.y = jumpVelocity;
        }
    }
}

void trippin::Goggin::onRising(Uint32 engineTicks) {
    if (velocity.y >= 0) {
        state = State::falling;
        channel.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
    }
}

void trippin::Goggin::onKeyDown() {
    Lock lock(mutex);
    channel.charge = true;
}

void trippin::Goggin::onKeyUp() {
    Lock lock(mutex);
    if (channel.charge) {
        channel.jump = true;
    }
}

double trippin::Goggin::findJumpVelocity(int ticks) const {
    auto range = static_cast<double>(maxJumpChargeTicks - minJumpChargeTicks);
    auto ratio = (std::max(minJumpChargeTicks, std::min(ticks, maxJumpChargeTicks)) - minJumpChargeTicks) / range;
    return minJumpVelocity + ratio * (maxJumpVelocity - minJumpVelocity);
}

trippin::Point<int> trippin::Goggin::getPosition() {
    Lock lock(mutex);
    return channel.roundedPosition;
}

int trippin::Goggin::getFrame() {
    Lock lock(mutex);
    return channel.frame;
}