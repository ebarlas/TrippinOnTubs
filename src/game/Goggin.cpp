#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "lock/Exchange.h"
#include "engine/Convert.h"

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
    maxFallingVelocity = 0;

    auto &ch = channel.ref();
    ch.position = roundedPosition;
    ch.center = toInt(center);
    ch.frame = FRAME_FALLING_LAST;

    for (auto &d : ch.dusts) {
        d.frame = dust->getFrames(); // past the end
    }

    ch.blast.frame = dustBlast->getFrames(); // past the end

    dustPeriodTicks = obj.dustPeriod;
    nextDustPos = 0;
}

void trippin::Goggin::setDust(const Sprite &spr) {
    dust = &spr;
};

void trippin::Goggin::setDustBlast(const trippin::Sprite &spr) {
    dustBlast = &spr;
}

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
            shrinkForDuck();
        }
    }

    if (ch.duckEnd) {
        ch.duckStart = ch.duckEnd = false;
        if (state == ducking) {
            ticks = 0;
            growForStand();
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
        auto range = toDouble(maxJumpChargeTicks - minJumpChargeTicks);
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
            if (state == ducking) {
                growForStand();
            }
            if (skipLaunch) {
                state = State::rising;
                ch.frame = FRAME_LAUNCHING_LAST;
                velocity.y = jumpVel;
                if (platformCollisions.testBottom() && jumpPercent >= 0.5) {
                    resetDustBlast(ch);
                }
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

    // advance dust blast
    if (ch.blast.frame < dustBlast->getFrames()) {
        ch.blast.ticks++;
        if (ch.blast.ticks == dustBlast->getFramePeriodTicks()) {
            ch.blast.ticks = 0;
            ch.blast.frame++;
        }
    }

    // test for creation of new dust cloud
    if (platformCollisions.testBottom() && engineTicks - dustTicks >= dustPeriodTicks) {
        dustTicks = engineTicks;
        auto left = roundedPosition.x;
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

    savePosition(ch);
}

void trippin::Goggin::savePosition(trippin::Goggin::Channel &ch) {
    if (state == ducking) {
        // restore y to normal in channel to prepare for rendering
        ch.position = {roundedPosition.x, toInt(position.y - size.y)};
        ch.center = {toInt(position.x + size.x / 2.0), toInt(position.y)};
    } else {
        ch.position = roundedPosition;
        ch.center = toInt(center);
    }
}

void trippin::Goggin::centerCamera(trippin::Camera &camera) {
    // record position here for use in subsequent render call to avoid jitter
    // jitter emerges when an engine tick updates the position *between* center and render calls
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();
    ch.cameraPosition = ch.position;
    camera.centerOn(ch.center);
}

void trippin::Goggin::onFalling(Uint32 engineTicks, Channel &ch) {
    if (velocity.y > maxFallingVelocity) {
        maxFallingVelocity = velocity.y;
    }

    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        state = State::landing;
        ticks = 0;
        ch.frame = FRAME_LANDING_FIRST;
        if (platformCollisions.testBottom() && maxFallingVelocity >= terminalVelocity.y / 2.0) {
            resetDustBlast(ch);
        }
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
        maxFallingVelocity = 0;
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
        maxFallingVelocity = 0;
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
        growForStand();
    }
}

void trippin::Goggin::shrinkForDuck() {
    // make goggin contact-area half height when ducking
    size.y /= 2.0;
    position.y += size.y;
    syncPositions();
}

void trippin::Goggin::growForStand() {
    // restore goggin contact area to full-height
    position.y -= size.y;
    size.y *= 2.0;
    syncPositions();
}

void trippin::Goggin::resetDustBlast(Channel &ch) {
    ch.blast.frame = 0;
    ch.blast.ticks = 0;
    ch.blast.position.x = (roundedPosition.x + size.x / 2) - (dustBlast->getHitBox().w / 2);
    ch.blast.position.y = (roundedPosition.y + size.y) - dustBlast->getHitBox().h;
}

void trippin::Goggin::render(const trippin::Camera &camera) {
    auto ch = channel.get();

    for (auto &d : ch.dusts) {
        if (d.frame < dust->getFrames()) {
            dust->render(d.position, d.frame, camera);
        }
    }

    if (ch.blast.frame < dustBlast->getFrames()) {
        dustBlast->render(ch.blast.position, ch.blast.frame, camera);
    }

    sprite->render(ch.cameraPosition, ch.frame, camera);
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
