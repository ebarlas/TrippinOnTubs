#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "Lock.h"

void trippin::Goggin::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);

    auto mul = scaleMultiplier(spr.getScale());
    auto gameTicksPerSecond = 1000.0 / config.tickPeriod;
    auto gameTicksPerSecondSq = gameTicksPerSecond * gameTicksPerSecond;

    skipLaunch = true;
    framePeriod = sprite->getDuration() / config.tickPeriod;
    runningAcceleration = (obj.runningAcceleration / gameTicksPerSecondSq) * mul;
    risingAcceleration = (obj.risingAcceleration / gameTicksPerSecondSq) * mul;
    minJumpVelocity = (obj.minJumpVelocity / gameTicksPerSecond) * mul;
    maxJumpVelocity = (obj.maxJumpVelocity / gameTicksPerSecond) * mul;
    minJumpChargeTicks = obj.minJumpChargeTime / config.tickPeriod;
    maxJumpChargeTicks = obj.maxJumpChargeTime / config.tickPeriod;
    jumpGracePeriodTicks = obj.jumpGracePeriod / config.tickPeriod;
    state = State::falling;
    channel.frame = 14;
}

void trippin::Goggin::beforeTick(const trippin::Clock &clock) {
    Lock lock(mutex);
    if (channel.charge && !chargeTicks) {
        chargeTicks = clock.getTicks();
    }
    if (channel.jump && chargeTicks) {
        int jumpTicks = clock.getTicks() - chargeTicks;
        channel.charge = false;
        channel.jump = false;
        chargeTicks = 0;
        if (state == running || (clock.getTicks() > lastRunTick && clock.getTicks() - lastRunTick < jumpGracePeriodTicks)) {
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

void trippin::Goggin::afterTick(const Clock &clock) {
    Lock lock(mutex);
    ticks++;

    if (state == State::falling) {
        onFalling(clock);
    } else if (state == State::landing) {
        onLanding(clock);
    } else if (state == State::running) {
        onRunning(clock);
    } else if (state == State::launching) {
        onLaunching(clock);
    } else if (state == State::rising) {
        onRising(clock);
    }

    channel.roundedPosition = roundedPosition;
    channel.roundedCenter = roundedCenter;
}

void trippin::Goggin::render(SDL_Renderer *renderer, const trippin::Camera &camera) {
    auto hb = sprite->getHitBox();
    auto size = sprite->getSize();
    auto viewport = camera.getViewport();
    auto ch = getChannel();
    trippin::Rect<int> box{ch.roundedPosition.x - hb.x, ch.roundedPosition.y - hb.y, size.x, size.y};
    if (box.intersect(viewport)) {
        Point<int> target = {box.x - viewport.x, box.y - viewport.y};
        sprite->render(target, ch.frame);
    }
}

trippin::Goggin::Channel trippin::Goggin::getChannel() {
    Lock lock(mutex);
    return channel;
}

void trippin::Goggin::center(trippin::Camera &camera) {
    auto chanel = getChannel();
    camera.centerOn(chanel.roundedCenter);
}

void trippin::Goggin::onFalling(const trippin::Clock &clock) {
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

void trippin::Goggin::onLanding(const trippin::Clock &clock) {
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

void trippin::Goggin::onRunning(const trippin::Clock &clock) {
    lastRunTick = clock.getTicks();

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

void trippin::Goggin::onLaunching(const trippin::Clock &clock) {
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

void trippin::Goggin::onRising(const trippin::Clock &clock) {
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
