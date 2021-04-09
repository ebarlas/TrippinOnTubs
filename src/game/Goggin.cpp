#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "engine/Convert.h"
#include "DigitLayout.h"

void trippin::Goggin::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);

    skipLaunch = true;
    runningAcceleration = obj.runningAcceleration;
    risingAcceleration = obj.risingAcceleration;
    minJumpVelocity = obj.minJumpVelocity;
    maxJumpVelocity = obj.maxJumpVelocity;
    maxDuckJumpVelocity = obj.maxDuckJumpVelocity;
    minJumpChargeTicks = obj.minJumpChargeTime;
    maxJumpChargeTicks = obj.maxJumpChargeTime;
    jumpGracePeriodTicks = obj.jumpGracePeriod;
    jumpSoundTimeoutTicks = obj.jumpSoundTimeout;
    duckFriction = obj.duckFriction;
    state = State::falling;
    maxFallingVelocity = 0;
    pointCloudDistanceMin = {size.x, size.y};
    pointCloudDistanceMax = pointCloudDistanceMin * 2;
    pointCloudTicks = config.ticksPerSecond() * 2;

    shakeAmplitude = config.shakeAmplitude * sprite->getScale().getMultiplier();

    // tick/ms * ms/s * s/shake = tick/shake
    auto shakePeriod = (1.0 / config.tickPeriod) * (1000.0 / config.shakeHertz);
    auto shakeDuration = config.shakeDuration / config.tickPeriod;
    xShake.init(toInt(shakePeriod), toInt(shakeDuration));
    yShake.init(toInt(shakePeriod), toInt(shakeDuration));

    for (auto &d : frames.dusts) {
        d.frame = dust->getFrames(); // past the end
    }
    frames.frame = FRAME_FALLING_LAST;
    frames.blast.frame = dustBlast->getFrames(); // past the end

    soundChannel.set({false});

    dustPeriodTicks = obj.dustPeriod;
    nextDustPos = 0;

    jumpSound = soundManager->getEffect("thud");

    syncChannel();
}

void trippin::Goggin::setUniverse(const trippin::Point<int> &uni) {
    universe = {0, 0, uni.x, uni.y};
}

void trippin::Goggin::setDust(const Sprite &spr) {
    dust = &spr;
};

void trippin::Goggin::setDustBlast(const Sprite &spr) {
    dustBlast = &spr;
}

void trippin::Goggin::setDigits(const Sprite &spr) {
    digits = &spr;
}

void trippin::Goggin::setSoundManager(SoundManager &sm) {
    soundManager = &sm;
}

void trippin::Goggin::setAutoPlay(const std::vector<GogginInputTick> &ap) {
    for (auto &uit : ap) {
        autoPlay[uit.tick] = {uit.jumpCharge, uit.jumpRelease, uit.duckStart, uit.duckEnd};
    }
    autoPlayEnabled = true;
}

void trippin::Goggin::beforeTick(Uint32 engineTicks) {
    transferInput(engineTicks);
    handleDuckStart();
    handleDuckEnd();
    handleJumpCharge(engineTicks);
    handleJumpRelease(engineTicks);
}

void trippin::Goggin::handleDuckStart() {
    if (input.duckStart) {
        if (state == running && platformCollisions.testBottom()) {
            state = ducking;
            ticks = 0;
            frames.frame = FRAME_DUCKING;
            acceleration.x = 0;
            friction.x = duckFriction;
            shrinkForDuck();
        }
    }
}

void trippin::Goggin::handleDuckEnd() {
    if (input.duckEnd) {
        input.duckStart = input.duckEnd = false;
        if (state == ducking) {
            ticks = 0;
            growForStand();
            if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
                state = running;
                frames.frame = FRAME_RUN_AFTER_LAND;
                acceleration.x = runningAcceleration;
            } else {
                state = falling;
                frames.frame = FRAME_FALLING_FIRST;
            }
        }
    }
}

void trippin::Goggin::handleJumpCharge(Uint32 engineTicks) {
    if (input.jumpCharge) {
        if (!jumpTicks) {
            jumpTicks = engineTicks;
        }
        auto relTicks = static_cast<int>(engineTicks - jumpTicks);
        auto range = toDouble(maxJumpChargeTicks - minJumpChargeTicks);
        auto boundedTicks = std::max(minJumpChargeTicks, std::min(relTicks, maxJumpChargeTicks));
        jumpPercent = (boundedTicks - minJumpChargeTicks) / range;
    } else {
        jumpPercent = 0;
    }
}

void trippin::Goggin::handleJumpRelease(Uint32 engineTicks) {
    if (input.jumpRelease && jumpTicks) {
        auto maxEffective = state == ducking && jumpPercent == 1.0 ? maxDuckJumpVelocity : maxJumpVelocity;
        double jumpVel = minJumpVelocity + jumpPercent * (maxEffective - minJumpVelocity);
        input.jumpCharge = false;
        input.jumpRelease = false;
        jumpTicks = 0;
        if (state == running || state == landing || state == ducking ||
            (engineTicks > lastRunOrDuckTick && engineTicks - lastRunOrDuckTick < jumpGracePeriodTicks)) {
            if (state == ducking) {
                growForStand();
            }
            if (skipLaunch) {
                maxFallingVelocity = 0;
                state = State::rising;
                frames.frame = FRAME_LAUNCHING_LAST;
                velocity.y = jumpVel;
                if (platformCollisions.testBottom() && jumpPercent >= 0.5) {
                    resetDustBlast();
                }
            } else {
                maxFallingVelocity = 0;
                state = State::launching;
                frames.frame = FRAME_LAUNCHING_FIRST;
                jumpVelocity = jumpVel;
            }
            enqueueJumpSound(engineTicks);
            acceleration.x = risingAcceleration;
            ticks = 0;
        }
    }
}

void trippin::Goggin::afterTick(Uint32 engineTicks) {
    ticks++;

    xShake.update(engineTicks);
    yShake.update(engineTicks);

    // advance dust cloud ticks
    for (auto &d : frames.dusts) {
        if (d.frame < dust->getFrames()) {
            d.ticks++;
            if (d.ticks == dust->getFramePeriodTicks()) {
                d.ticks = 0;
                d.frame++; // may go past last frame, denoting inactive
            }
        }
    }

    // advance dust blast
    if (frames.blast.frame < dustBlast->getFrames()) {
        frames.blast.ticks++;
        if (frames.blast.ticks == dustBlast->getFramePeriodTicks()) {
            frames.blast.ticks = 0;
            frames.blast.frame++;
        }
    }

    // test for creation of new dust cloud
    if (platformCollisions.testBottom() && engineTicks - dustTicks >= dustPeriodTicks) {
        dustTicks = engineTicks;
        auto left = roundedPosition.x;
        auto top = roundedPosition.y + size.y - dust->getHitBox().h;
        frames.dusts[nextDustPos] = {{left, top}, 0};
        nextDustPos = (nextDustPos + 1) % frames.dusts.size();
    }

    // advance point clouds
    for (auto &pc : pointClouds) {
        auto elapsed = engineTicks - pc.ticks;
        float di = decelInterpolation(std::min(1.0f, elapsed / (float) pointCloudTicks));
        if (di == 1.0) {
            pc.points = 0; // cancel display
        } else {
            pc.posNow.x = pc.posStart.x + toInt(di * pc.distance.x); // x diff may be pos (right) or neg (left)
            pc.posNow.y = pc.posStart.y - toInt(di * pc.distance.y); // y diff is always negative (up)
        }
    }

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
    } else if (state == State::ducking) {
        onDucking(engineTicks);
    }

    if (!universe.intersect(roundedBox)) {
        expired = true;
    }

    syncChannel();
}

void trippin::Goggin::centerCamera(trippin::Camera &camera) {
    // record position here for use in subsequent render call to avoid jitter
    // jitter emerges when an engine tick updates the position *between* center and render calls
    auto ch = channel.get();
    cameraPosition = ch.position;
    camera.centerOn(ch.center);
}

void trippin::Goggin::onFalling(Uint32 engineTicks) {
    if (velocity.y > maxFallingVelocity) {
        maxFallingVelocity = velocity.y;
    }

    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        state = State::landing;
        ticks = 0;
        frames.frame = FRAME_LANDING_FIRST;
        acceleration.x = runningAcceleration;
        if (platformCollisions.testBottom() && maxFallingVelocity >= terminalVelocity.y / 2.0) {
            resetDustBlast();
            xShake.start(engineTicks);
            yShake.start(engineTicks);
        }
        enqueueJumpSound(engineTicks);
        return;
    }

    if (ticks == sprite->getFramePeriodTicks()) {
        ticks = 0;
        auto frame = frames.frame;
        if (frame < FRAME_FALLING_LAST) {
            frames.frame = frame + 1;
        }
    }
}

void trippin::Goggin::onLanding(Uint32 engineTicks) {
    if (ticks != sprite->getFramePeriodTicks()) {
        return;
    }

    ticks = 0;
    auto frame = frames.frame;
    if (frame == FRAME_LANDING_FIRST) {
        frames.frame = frame + 1;
        return;
    }

    state = running;
    frames.frame = FRAME_RUN_AFTER_LAND;
    acceleration.x = runningAcceleration;
}

void trippin::Goggin::onRunning(Uint32 engineTicks) {
    lastRunOrDuckTick = engineTicks;

    if (!platformCollisions.testBottom() && !objectCollisions.testBottom()) {
        state = State::falling;
        frames.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        maxFallingVelocity = 0;
        return;
    }

    if (ticks == sprite->getFramePeriodTicks()) {
        ticks = 0;
        frames.frame = (frames.frame + 1) % RUNNING_FRAMES;
    }
}

void trippin::Goggin::onLaunching(Uint32 engineTicks) {
    if (ticks != sprite->getFramePeriodTicks()) {
        return;
    }

    ticks = 0;
    auto frame = frames.frame;
    if (frame < FRAME_LAUNCHING_LAST) {
        frame++;
        frames.frame = frame;
        if (frame == FRAME_LAUNCHING_LAST) {
            state = State::rising;
            velocity.y = jumpVelocity;
        }
    }
}

void trippin::Goggin::onRising(Uint32 engineTicks) {
    if (velocity.y >= 0) {
        state = State::falling;
        frames.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        maxFallingVelocity = 0;
    }
}

void trippin::Goggin::onDucking(Uint32 engineTicks) {
    lastRunOrDuckTick = engineTicks;

    if (!platformCollisions.testBottom()) {
        ticks = 0;
        if (objectCollisions.testBottom()) {
            state = running;
            frames.frame = FRAME_RUN_AFTER_LAND;
            acceleration.x = runningAcceleration;
        } else {
            state = falling;
            frames.frame = FRAME_FALLING_FIRST;
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

void trippin::Goggin::resetDustBlast() {
    frames.blast.frame = 0;
    frames.blast.ticks = 0;
    frames.blast.position.x = (roundedPosition.x + size.x / 2) - (dustBlast->getHitBox().w / 2);
    frames.blast.position.y = (roundedPosition.y + size.y) - dustBlast->getHitBox().h;
}

void trippin::Goggin::render(const Camera &camera) {
    auto ch = channel.get();
    if (ch.expired) {
        return;
    }

    for (auto &d : ch.frames.dusts) {
        if (d.frame < dust->getFrames()) {
            dust->render(d.position, d.frame, camera);
        }
    }

    if (ch.frames.blast.frame < dustBlast->getFrames()) {
        dustBlast->render(ch.frames.blast.position, ch.frames.blast.frame, camera);
    }

    sprite->render(cameraPosition, ch.frames.frame, camera);

    auto size = sprite->getSize();
    for (auto &pc : ch.pointClouds) {
        if (pc.points) {
            Point<int> p{pc.posNow.x - size.x / 2, pc.posNow.y - size.y};
            DigitLayout::renderDigits(*digits, p, pc.points, &camera);
        }
    }

    auto soundCh = soundChannel.get();
    if (soundCh.playJumpSound) {
        Mix_PlayChannel(-1, jumpSound, 0);
        soundChannel.set({false});
    }
}

void trippin::Goggin::onUserInput(const trippin::GogginInput &in) {
    inputChannel.set(in);
}

double trippin::Goggin::getJumpCharge() const {
    return jumpPercent;
}

void trippin::Goggin::enqueueJumpSound(Uint32 engineTicks) {
    if (engineTicks - lastJumpTicks >= jumpSoundTimeoutTicks) {
        soundChannel.set({true});
        lastJumpTicks = engineTicks;
    }
}

bool trippin::Goggin::inUniverse() const {
    return !channel.get().expired;
}

void trippin::Goggin::transferInput(Uint32 engineTicks) {
    if (autoPlayEnabled) {
        auto it = autoPlay.find(engineTicks);
        if (it != autoPlay.end()) {
            input |= it->second;
        }
        return;
    }

    auto ch = inputChannel.getAndSet({false, false, false, false});
    auto prev = input;
    input |= ch;
    if (input != prev) {
        SDL_Log("input event, ticks=%d, duckStart=%d, duckEnd=%d, jumpCharge=%d, jumpRelease=%d",
                engineTicks, ch.duckStart, ch.duckEnd, ch.jumpCharge, ch.jumpRelease);
    }
}

void trippin::Goggin::syncChannel() {
    Channel ch;
    Point<int> shake{toInt(xShake.amplitude() * shakeAmplitude), toInt(yShake.amplitude() * shakeAmplitude)};
    if (state == ducking) {
        // restore y to normal in channel to prepare for rendering
        ch.position = {roundedPosition.x, toInt(position.y - size.y)};
        ch.center = Point<int>({toInt(position.x + size.x / 2.0), toInt(position.y)}) + shake;
    } else {
        ch.position = roundedPosition;
        ch.center = toInt(center) + shake;
    }
    ch.frames = frames;
    ch.expired = expired;
    ch.pointClouds = pointClouds;
    channel.set(ch);
}

float trippin::Goggin::decelInterpolation(float input) {
    return (float) (1.0f - (1.0f - input) * (1.0f - input));;
}

void trippin::Goggin::addPointCloud(int points, Uint32 ticks) {
    int numDigits = DigitLayout::countDigits(*digits, points);
    int x = roundedBox.x + roundedBox.w / 2 + (numDigits * digits->getSize().x) / 2; // goggin horiz. midpoint
    int y = roundedBox.y;
    int xRange = pointCloudDistanceMax.x - pointCloudDistanceMin.x;
    int yRange = pointCloudDistanceMax.y - pointCloudDistanceMin.y;
    double xRand = ((std::rand() * 2.0) / RAND_MAX - 1.0); // [-1.0, 1.0]
    double yRand = (std::rand() * 1.0) / RAND_MAX; // [0.0, 1.0]
    auto xDist = pointCloudDistanceMin.x + toInt(xRand * xRange);
    auto yDist = pointCloudDistanceMin.y + toInt(yRand * yRange);
    pointClouds[nextPointCloudPos] = {{x, y}, {x, y}, {xDist, yDist}, points, ticks};
    nextPointCloudPos = (nextPointCloudPos + 1) % pointClouds.size();
}

bool trippin::Goggin::maxFallingVelocityAbove(double percent) {
    return (maxFallingVelocity / terminalVelocity.y) > percent;
}
