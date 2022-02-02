#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "engine/Convert.h"

trippin::Goggin::Goggin(
        const Configuration &config,
        const Map::Object &object,
        SpriteManager &spriteManager,
        ComboManager &comboManager,
        PointCloudManager &pointCloudManager,
        const std::vector<GogginInputTick> *autoPlayVec,
        const trippin::Point<int> &universe,
        SoundManager &soundManager,
        Camera &camera,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        SpriteObject(config, object, spriteManager.get("goggin")),
        dust(spriteManager.get("dust")),
        dustBlast(spriteManager.get("dust_blast")),
        whiteDustBlast(spriteManager.get("dust_blast_white")),
        comboManager(comboManager),
        pointCloudManager(pointCloudManager),
        universe(universe),
        camera(camera),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        jumpSound(soundManager.getEffect("thud")),
        skipLaunch(true),
        runningAcceleration(object.runningAcceleration),
        risingAcceleration(object.risingAcceleration),
        minJumpVelocity(object.minJumpVelocity),
        maxJumpVelocity(object.maxJumpVelocity),
        maxDuckJumpVelocity(object.maxDuckJumpVelocity),
        minJumpChargeTicks(object.minJumpChargeTime),
        maxJumpChargeTicks(object.maxJumpChargeTime),
        jumpGracePeriodTicks(object.jumpGracePeriod),
        jumpSoundTimeoutTicks(object.jumpSoundTimeout),
        duckFriction(object.duckFriction),
        shakeAmplitude(config.shakeAmplitude * sprite.getScale().getMultiplier()),
        dustPeriodTicks(object.dustPeriod) {

    rightOfUni = false;
    belowUni = false;

    grounded = false;
    state = State::falling;
    maxFallingVelocity = 0;
    consecutiveJumps = 0;

    // tick/ms * ms/s * s/shake = tick/shake
    auto shakePeriod = (1.0 / config.msPerTick()) * (1000.0 / config.shakeHertz);
    auto shakeDuration = config.shakeDuration / config.msPerTick();
    xShake.init(toInt(shakePeriod), toInt(shakeDuration));
    yShake.init(toInt(shakePeriod), toInt(shakeDuration));

    for (auto &d: frames.dusts) {
        d.frame = dust.getFrames(); // past the end
    }

    frames.frame = FRAME_FALLING_LAST;
    frames.blast.frame = dustBlast.getFrames(); // past the end

    dustTicks = 0;
    nextDustPos = 0;
    jumpVelocity = 0;

    rememberDuckStart = false;

    lastJumpTicks = 0;
    lastDuckTicks = 0;
    lastChargedJumpTicks = 0;
    lastDuckJumpTicks = 0;
    lastDoubleJumpTicks = 0;
    lastJumpSoundTicks = 0;

    if (autoPlayVec != nullptr) {
        for (auto &uit: *autoPlayVec) {
            autoPlay[uit.tick] = {uit.jumpCharge, uit.jumpRelease, uit.duckStart, uit.duckEnd};
        }
        autoPlayEnabled = true;
    } else {
        autoPlayEnabled = false;
    }
}

void trippin::Goggin::beforeTick(Uint32 engineTicks) {
    transferInput(engineTicks);
    handleDuckStart(engineTicks);
    handleDuckEnd();
    handleJumpCharge(engineTicks);
    handleJumpRelease(engineTicks);
}

void trippin::Goggin::handleDuckStart(Uint32 engineTicks) {
    if (input.duckStart) {
        rememberDuckStart = true;
    }
    if (rememberDuckStart) {
        if ((state == running || state == landing) && platformCollisions.testBottom()) {
            rememberDuckStart = false;
            state = ducking;
            lastDuckTicks = engineTicks;
            ticks = 0;
            frames.frame = FRAME_DUCKING;
            acceleration.x = 0;
            friction.x = duckFriction;
            shrinkForDuck();
        } else if (state == rising || state == falling) {
            velocity.y = terminalVelocity.y;
        }
    }
}

void trippin::Goggin::handleDuckEnd() {
    if (input.duckEnd) {
        rememberDuckStart = false;
        if (state == ducking) {
            ticks = 0;
            growForStand();
            friction.x = 0;
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
        jumpTicks = engineTicks;
    }
    if (jumpTicks) {
        auto relTicks = static_cast<int>(engineTicks - jumpTicks);
        auto range = toDouble(maxJumpChargeTicks - minJumpChargeTicks);
        auto boundedTicks = std::max(minJumpChargeTicks, std::min(relTicks, maxJumpChargeTicks));
        jumpPercent = (boundedTicks - minJumpChargeTicks) / range;
    }
}

void trippin::Goggin::handleJumpRelease(Uint32 engineTicks) {
    if (input.jumpRelease && jumpTicks) {
        auto maxEffective = state == ducking && jumpPercent == 1.0 ? maxDuckJumpVelocity : maxJumpVelocity;
        double jumpVel = minJumpVelocity + jumpPercent * (maxEffective - minJumpVelocity);
        if (state == running || state == landing || state == ducking ||
            ((state == falling || state == rising) && consecutiveJumps < 2) ||
            (engineTicks > lastRunOrDuckTick && engineTicks - lastRunOrDuckTick < jumpGracePeriodTicks)) {
            if (state == ducking) {
                growForStand();
            }
            if (skipLaunch) {
                if ((platformCollisions.testBottom() && jumpPercent >= 0.5) ||
                    ((state == falling || state == rising) && consecutiveJumps < 2)) {
                    resetDustBlast(state == falling || state == rising);
                }
                maxFallingVelocity = 0;
                state = State::rising;
                frames.frame = FRAME_LAUNCHING_LAST;
                velocity.y = jumpVel;
            } else {
                maxFallingVelocity = 0;
                state = State::launching;
                frames.frame = FRAME_LAUNCHING_FIRST;
                jumpVelocity = jumpVel;
            }
            consecutiveJumps++;
            if (consecutiveJumps == 2) {
                lastDoubleJumpTicks = engineTicks;
            }
            lastJumpTicks = engineTicks;
            if (jumpPercent == 1.0) {
                if (maxEffective == maxDuckJumpVelocity) {
                    lastDuckJumpTicks = engineTicks;
                } else {
                    lastChargedJumpTicks = engineTicks;
                }
            }
            playJumpSound(engineTicks);
            acceleration.x = risingAcceleration;
            ticks = 0;
        }
        jumpTicks = 0;
        jumpPercent = 0;
    }
}

void trippin::Goggin::afterTick(Uint32 engineTicks) {
    ticks++;

    xShake.update(engineTicks);
    yShake.update(engineTicks);

    // advance dust cloud ticks
    for (auto &d: frames.dusts) {
        if (d.frame < dust.getFrames()) {
            d.ticks++;
            if (d.ticks == dust.getFramePeriodTicks()) {
                d.ticks = 0;
                d.frame++; // may go past last frame, denoting inactive
            }
        }
    }

    // advance dust blast
    if (frames.blast.frame < dustBlast.getFrames()) {
        frames.blast.ticks++;
        if (frames.blast.ticks == dustBlast.getFramePeriodTicks()) {
            frames.blast.ticks = 0;
            frames.blast.frame++;
        }
    }

    grounded = platformCollisions.testBottom();
    if (grounded) {
        comboManager.reset();
    }

    // test for creation of new dust cloud
    if (grounded && engineTicks - dustTicks >= dustPeriodTicks && velocity.x >= terminalVelocity.x / 2) {
        dustTicks = engineTicks;
        auto left = roundedPosition.x;
        auto top = roundedPosition.y + size.y - dust.getHitBox().h;
        frames.dusts[nextDustPos] = {{left, top}, 0};
        nextDustPos = (nextDustPos + 1) % frames.dusts.size();
    }

    if (grounded) {
        consecutiveJumps = 0;
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

    if (position.x >= universe.x) {
        rightOfUni = true;
        expired = true;
    }
    if (position.y >= universe.y) {
        belowUni = true;
        expired = true;
    }

    auto cameraPos = centerCamera();

    drawDust();
    drawDustBlast();

    auto frameNow = frames.frame;
    sceneBuilder.dispatch([this, cameraPos, frameNow]() {
        sprite.render(cameraPos, frameNow, camera);
    }, zIndex);
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
            resetDustBlast(false);
            xShake.start(engineTicks);
            yShake.start(engineTicks);
        }
        playJumpSound(engineTicks);
        return;
    }

    if (ticks == sprite.getFramePeriodTicks()) {
        ticks = 0;
        auto frame = frames.frame;
        if (frame < FRAME_FALLING_LAST) {
            frames.frame = frame + 1;
        }
    }
}

void trippin::Goggin::onLanding(Uint32 engineTicks) {
    if (ticks != sprite.getFramePeriodTicks()) {
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

    if (ticks == sprite.getFramePeriodTicks()) {
        ticks = 0;
        frames.frame = (frames.frame + 1) % RUNNING_FRAMES;
    }
}

void trippin::Goggin::onLaunching(Uint32 engineTicks) {
    if (ticks != sprite.getFramePeriodTicks()) {
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
    // reduce goggin contact-area half height when ducking
    int bottom = roundedPosition.y + size.y;
    size.y = toInt(size.y * DUCK_HEIGHT);

    // fill gap under reduced height
    int delta = bottom - toInt(size.y + position.y);
    position.y += delta;

    syncPositions();
}

void trippin::Goggin::growForStand() {
    // restore goggin contact area to full-height using original sprite height
    position.y -= sprite.getHitBox().h - size.y;
    size.y = sprite.getHitBox().h;
    syncPositions();
}

void trippin::Goggin::resetDustBlast(bool white) {
    frames.blast.white = white;
    frames.blast.frame = 0;
    frames.blast.ticks = 0;
    frames.blast.position.x = (roundedPosition.x + size.x / 2) - (dustBlast.getHitBox().w / 2);
    frames.blast.position.y = (roundedPosition.y + size.y) - dustBlast.getHitBox().h;
}

void trippin::Goggin::onUserInput(const trippin::GogginInput &in) {
    std::lock_guard<std::mutex> lock(mutex);
    stagedInput |= in;
}

double trippin::Goggin::getJumpCharge() const {
    return jumpPercent;
}

void trippin::Goggin::playJumpSound(Uint32 engineTicks) {
    if (engineTicks - lastJumpSoundTicks >= jumpSoundTimeoutTicks) {
        Mix_PlayChannel(-1, jumpSound, 0);
    }
}

bool trippin::Goggin::rightOfUniverse() const {
    return rightOfUni;
}

bool trippin::Goggin::belowUniverse() const {
    return belowUni;
}

void trippin::Goggin::transferInput(Uint32 engineTicks) {
    if (autoPlayEnabled) {
        auto it = autoPlay.find(engineTicks);
        if (it != autoPlay.end()) {
            input = it->second;
        } else {
            input = {};
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        input = std::exchange(stagedInput, {});
    }

    if (input) {
        SDL_Log("input event, ticks=%d, duckStart=%d, duckEnd=%d, jumpCharge=%d, jumpRelease=%d",
                engineTicks, input.duckStart, input.duckEnd, input.jumpCharge, input.jumpRelease);
    }
}

void trippin::Goggin::addPointCloud(int points, Uint32 ticks, bool hit) {
    Point<int> pos{roundedBox.x + roundedBox.w / 2, roundedBox.y - size.y};
    pointCloudManager.addPointCloud(pos, points, ticks);
    if (hit && !grounded) {
        comboManager.recordHit();
    }
}

Uint32 trippin::Goggin::getLastJumpTicks() const {
    return lastJumpTicks;
}

Uint32 trippin::Goggin::getLastDuckTicks() const {
    return lastDuckTicks;
}

Uint32 trippin::Goggin::getLastChargedJumpTicks() const {
    return lastChargedJumpTicks;
}

Uint32 trippin::Goggin::getLastDuckJumpTicks() const {
    return lastDuckJumpTicks;
}

Uint32 trippin::Goggin::getLastDoubleJumpTicks() const {
    return lastDoubleJumpTicks;
}

trippin::Point<int> trippin::Goggin::centerCamera() {
    Point<int> shake{toInt(xShake.amplitude() * shakeAmplitude), toInt(yShake.amplitude() * shakeAmplitude)};
    Point<int> pos;
    Point<int> cen;
    if (state == ducking) {
        // restore y to normal in channel to prepare for rendering
        auto heightDelta = sprite.getHitBox().h - size.y;
        pos = {roundedPosition.x, toInt(position.y - heightDelta)};
        cen = Point<int>({toInt(position.x + size.x / 2.0), toInt(position.y)});
    } else {
        pos = roundedPosition;
        cen = toInt(center);
    }
    camera.centerOn(cen, shake);
    return pos;
}

void trippin::Goggin::drawDust() {
    for (auto &d: frames.dusts) {
        if (d.frame < dust.getFrames()) {
            auto dustPos = d.position;
            auto dustFrame = d.frame;
            sceneBuilder.dispatch([this, dustPos, dustFrame]() {
                dust.render(dustPos, dustFrame, camera);
            }, zIndex);
        }
    }
}

void trippin::Goggin::drawDustBlast() {
    if (frames.blast.frame < dustBlast.getFrames()) {
        auto dustSprite = frames.blast.white ? &whiteDustBlast : &dustBlast;
        auto dustPos = frames.blast.position;
        auto dustFrame = frames.blast.frame;
        sceneBuilder.dispatch([this, dustSprite, dustPos, dustFrame]() {
            dustSprite->render(dustPos, dustFrame, camera);
        }, zIndex);
    }
}