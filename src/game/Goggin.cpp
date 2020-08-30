#include "sprite/Sprite.h"
#include "Goggin.h"
#include "Lock.h"

void trippin::Goggin::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);

    auto mul = scaleMultiplier(spr.getScale());
    auto gameTicksPerSecond = 1000.0 / config.tickPeriod;
    auto gameTicksPerSecondSq = gameTicksPerSecond * gameTicksPerSecond;

    platform = false;
    activatedAcceleration = (obj.activatedAcceleration / gameTicksPerSecondSq) * mul;
    jumpVelocity = (obj.jumpVelocity / gameTicksPerSecond) * mul;
    framePeriod = static_cast<int>(sprite->getDuration() / config.tickPeriod);
    state = State::falling;
    channel.frame = 14;
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
    acceleration.x = activatedAcceleration;
}

void trippin::Goggin::onRunning(const trippin::Clock &clock) {
    if (!platformCollisions.testBottom()) {
        state = State::falling;
        channel.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        return;
    }

    if (std::rand() % 200 == 0) {
        state = State::launching;
        channel.frame = FRAME_LAUNCHING_FIRST;
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
    }
}
