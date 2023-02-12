#include <algorithm>
#include "sprite/Sprite.h"
#include "Goggin.h"
#include "engine/Convert.h"

trippin::Goggin::Goggin(
        const Configuration &config,
        const Configuration::Object &configObject,
        const Map::Object &object,
        SpriteManager &spriteManager,
        ComboManager &comboManager,
        PointCloudManager &pointCloudManager,
        const std::vector<GogginInputTick> *autoPlayVec,
        const trippin::Point<int> &universe,
        SoundManager &soundManager,
        Camera &camera,
        SceneBuilder &sceneBuilder) :
        SpriteObject(configObject, object, spriteManager.get("goggin")),
        config(config),
        dust(spriteManager.get("dust")),
        dustPeriodTicks(configObject.dustPeriod),
        dustBlast(spriteManager.get("dust_blast")),
        whiteDustBlast(spriteManager.get("dust_blast_white")),
        universe(universe),
        risingAcceleration(configObject.risingAcceleration),
        runningAcceleration(configObject.runningAcceleration),
        duckFriction(configObject.duckFriction),
        minJumpVelocity(configObject.minJumpVelocity),
        maxJumpVelocity(configObject.maxJumpVelocity),
        maxDuckJumpVelocity(configObject.maxDuckJumpVelocity),
        minJumpChargeTicks(configObject.minJumpChargeTime),
        maxJumpChargeTicks(configObject.maxJumpChargeTime),
        jumpSound(soundManager.getEffect(config.jumpSound)),
        jumpSoundTimeoutTicks(configObject.jumpSoundTimeout),
        pointCloudManager(pointCloudManager),
        comboManager(comboManager),
        sceneBuilder(sceneBuilder),
        camera(camera) {

    rightOfUni = false;
    belowUni = false;

    grounded = false;
    state = State::falling;
    maxFallingVelocity = 0;
    consecutiveJumps = 0;

    auto shakePeriod = config.ticksPerSecond() / config.shakeHertz; // (ticks/sec) / (shake/sec) = ticks/shake
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

    rememberDuckStart = false;

    lastJumpTicks = 0;
    lastDuckTicks = 0;
    lastChargedJumpTicks = 0;
    lastDuckJumpTicks = 0;
    lastDoubleJumpTicks = 0;
    lastJumpSoundTicks = 0;
    lastStopTicks = 0;
    lastJumpSlamDownTicks = 0;

    if (autoPlayVec != nullptr) {
        for (auto &uit: *autoPlayVec) {
            autoPlay[uit.tick] = {uit.jumpCharge, uit.jumpRelease, uit.duckStart, uit.duckEnd};
        }
        autoPlayEnabled = true;
    } else {
        autoPlayEnabled = false;
    }
}

void trippin::Goggin::beforeTick(int engineTicks) {
    transferInput(engineTicks);
    handleDuckStart(engineTicks);
    handleDuckEnd();
    handleJumpCharge(engineTicks);
    handleJumpRelease(engineTicks);
}

void trippin::Goggin::handleDuckStart(int engineTicks) {
    if (input.duckStart) {
        rememberDuckStart = true;
    }
    if (rememberDuckStart) {
        if (state == running && platformCollisions.testBottom()) {
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
            lastJumpSlamDownTicks = engineTicks;
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

void trippin::Goggin::handleJumpCharge(int engineTicks) {
    if (input.jumpCharge) {
        jumpTicks = engineTicks;
    }
    if (jumpTicks) {
        auto relTicks = engineTicks - jumpTicks;
        double range = maxJumpChargeTicks - minJumpChargeTicks;
        auto boundedTicks = std::max(minJumpChargeTicks, std::min(relTicks, maxJumpChargeTicks));
        jumpPercent = (boundedTicks - minJumpChargeTicks) / range;
    }
}

void trippin::Goggin::handleJumpRelease(int engineTicks) {
    if (input.jumpRelease && jumpTicks) {
        auto maxEffective = state == ducking && jumpPercent == 1.0 ? maxDuckJumpVelocity : maxJumpVelocity;
        auto jumpVel = std::min(minJumpVelocity, jumpPercent * maxEffective);
        if (state == running || state == ducking || consecutiveJumps < config.maxConsecutiveJumps) {
            if (state == ducking) {
                growForStand();
            }
            if ((platformCollisions.testBottom() && jumpPercent >= config.dustJumpFactor) ||
                ((state == falling || state == rising) && consecutiveJumps < config.maxConsecutiveJumps)) {
                resetDustBlast(state == falling || state == rising);
            }
            maxFallingVelocity = 0;
            state = State::rising;
            frames.frame = FRAME_LAUNCHING_LAST;
            velocity.y = jumpVel;
            consecutiveJumps++;
            if (consecutiveJumps == config.maxConsecutiveJumps) {
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

void trippin::Goggin::afterTick(int engineTicks) {
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
        int hits = comboManager.reset();
        if (hits) {
            addPointCloud(hits * config.pointsPerComboHit, engineTicks);
        }
    }

    // test for creation of new dust cloud
    auto dustTicksElapsed = engineTicks - dustTicks;
    if (grounded && dustTicksElapsed >= dustPeriodTicks && velocity.x >= terminalVelocity.x * config.dustRunFactor) {
        dustTicks = engineTicks;
        auto left = roundedPosition.x - dust.getEngineHitBox().w / 2; // horizontally center dust on goggin left
        auto top = roundedPosition.y + size.y - dust.getEngineHitBox().h;
        frames.dusts[nextDustPos] = {{left, top}, 0, 0, false};
        nextDustPos = (nextDustPos + 1) % frames.dusts.size();
    }

    if (grounded) {
        consecutiveJumps = 0;
    }

    if (state == State::falling) {
        onFalling(engineTicks);
    } else if (state == State::running) {
        onRunning(engineTicks);
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

    cameraPos = centerCamera();
}

void trippin::Goggin::render() {
    drawDust();
    drawDustBlast();

    auto pos = cameraPos;
    auto frameNow = frames.frame;
    auto vp = camera.getViewport();
    sceneBuilder.dispatch([this, pos, frameNow, vp]() {
        sprite.renderEngine(pos, frameNow, vp);
    });
}

void trippin::Goggin::onFalling(int engineTicks) {
    if (velocity.y > maxFallingVelocity) {
        maxFallingVelocity = velocity.y;
    }

    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        state = State::running;
        ticks = 0;
        frames.frame = FRAME_RUN_AFTER_LAND;
        acceleration.x = runningAcceleration;
        if (platformCollisions.testBottom() && maxFallingVelocity >= terminalVelocity.y * config.dustLandFactor) {
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

void trippin::Goggin::onRunning(int) {
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

void trippin::Goggin::onRising(int) {
    if (velocity.y >= 0) {
        state = State::falling;
        frames.frame = FRAME_FALLING_FIRST;
        ticks = 0;
        acceleration.x = 0;
        maxFallingVelocity = 0;
    }
}

void trippin::Goggin::onDucking(int engineTicks) {
    if (velocity.x == 0) {
        lastStopTicks = engineTicks;
    }

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
    position.y -= sprite.getEngineHitBox().h - size.y;
    size.y = sprite.getEngineHitBox().h;
    syncPositions();
}

void trippin::Goggin::resetDustBlast(bool white) {
    frames.blast.white = white;
    frames.blast.frame = 0;
    frames.blast.ticks = 0;
    frames.blast.position.x = (roundedPosition.x + size.x / 2) - (dustBlast.getEngineHitBox().w / 2);
    frames.blast.position.y = (roundedPosition.y + size.y) - dustBlast.getEngineHitBox().h;
}

void trippin::Goggin::onUserInput(const trippin::GogginInput &in) {
    std::lock_guard<std::mutex> lock(mutex);
    stagedInput |= in;
}

double trippin::Goggin::getJumpCharge() const {
    return jumpPercent;
}

void trippin::Goggin::playJumpSound(int engineTicks) {
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

void trippin::Goggin::transferInput(int engineTicks) {
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
        {
            std::lock_guard<std::mutex> lock(mutex);
            inputEvents.push_back({{input.jumpCharge, input.jumpRelease, input.duckStart, input.duckEnd}, engineTicks});
        }
    }
}

void trippin::Goggin::addPointCloud(int points, int engineTicks, bool hit) {
    Point<int> pos{roundedBox.x + roundedBox.w / 2, roundedBox.y - size.y};
    pointCloudManager.addPointCloud(pos, points, engineTicks);
    if (hit && !grounded) {
        comboManager.recordHit();
    }
}

int trippin::Goggin::getLastJumpTicks() const {
    return lastJumpTicks;
}

int trippin::Goggin::getLastDuckTicks() const {
    return lastDuckTicks;
}

int trippin::Goggin::getLastChargedJumpTicks() const {
    return lastChargedJumpTicks;
}

int trippin::Goggin::getLastDuckJumpTicks() const {
    return lastDuckJumpTicks;
}

int trippin::Goggin::getLastDoubleJumpTicks() const {
    return lastDoubleJumpTicks;
}

int trippin::Goggin::getLastStopTicks() const {
    return lastStopTicks;
}

int trippin::Goggin::getLastJumpSlamDownTicks() const {
    return lastJumpSlamDownTicks;
}

trippin::Point<int> trippin::Goggin::centerCamera() {
    Point<int> shake{
            static_cast<int>(static_cast<double>(xShake.amplitude()) * config.shakeAmplitude),
            static_cast<int>(static_cast<double>(yShake.amplitude()) * config.shakeAmplitude)};
    Point<int> pos;
    Point<int> cen;
    if (state == ducking) {
        // restore y to original footprint by lifting sprite
        auto originalHeight = sprite.getEngineHitBox().h;
        auto heightDelta = originalHeight - size.y;
        pos = {roundedPosition.x, roundedPosition.y - heightDelta};
        cen = toInt(Point<double>{pos.x + size.x / 2.0, pos.y + originalHeight / 2.0});
    } else {
        pos = roundedPosition;
        cen = toInt(center);
    }
    camera.centerOn(cen, shake);
    return pos;
}

void trippin::Goggin::drawDust() {
    auto vp = camera.getViewport();
    for (auto &d: frames.dusts) {
        if (d.frame < dust.getFrames()) {
            auto dustPos = d.position;
            auto dustFrame = d.frame;
            sceneBuilder.dispatch([this, dustPos, dustFrame, vp]() {
                dust.renderEngine(dustPos, dustFrame, vp);
            });
        }
    }
}

void trippin::Goggin::drawDustBlast() {
    auto vp = camera.getViewport();
    if (frames.blast.frame < dustBlast.getFrames()) {
        auto dustSprite = frames.blast.white ? &whiteDustBlast : &dustBlast;
        auto dustPos = frames.blast.position;
        auto dustFrame = frames.blast.frame;
        sceneBuilder.dispatch([dustSprite, dustPos, dustFrame, vp]() {
            dustSprite->renderEngine(dustPos, dustFrame, vp);
        });
    }
}

std::vector<trippin::GogginInputTick> trippin::Goggin::takeInputEvents() {
    std::lock_guard<std::mutex> lock(mutex);
    return std::move(inputEvents);
}
