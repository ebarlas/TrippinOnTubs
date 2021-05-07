#include "GameObject.h"
#include "engine/Convert.h"
#include "engine/Collisions.h"

void trippin::GameObject::setGoggin(Goggin &g) {
    goggin = &g;
}

void trippin::GameObject::setScoreTicker(ScoreTicker &st) {
    scoreTicker = &st;
}

void trippin::GameObject::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    accelerateWhenGrounded = obj.accelerateWhenGrounded;
    if (accelerateWhenGrounded) {
        runningAcceleration = obj.runningAcceleration;
    } else {
        acceleration.x = obj.runningAcceleration;
    }
    stompable = obj.stompable;
    stompPoints = obj.points;
    objectActivation = obj.activation;
    stomped = false;
    if (obj.randFrame) {
        frame = std::rand() % spr.getFrames() / 2;
    } else {
        frame = obj.frame;
    }
    collisionDuration = config.ticksPerSecond() * 0.25;
    collisionTicks = 0;
    flashCycle = 0;
    if (obj.coefficient > 0) {
        auto coefficient = obj.coefficient;
        platformCollision.set([coefficient](Object &left, Object &right, const Sides &sides) {
            onReflectiveCollision(left, right, sides, coefficient);
        });
    }
    if (obj.elasticObjectCollisions) {
        objectCollision.set(onElasticCollision2D);
    }
    syncChannel(0);
}

void trippin::GameObject::beforeTick(Uint32 engineTicks) {
    if (inactive) {
        if (objectActivation > 0) {
            if (goggin->position.x >= position.x - objectActivation) {
                inactive = false;
            }
        } else {
            if (activation->shouldActivate(roundedBox)) {
                inactive = false;
            }
        }
    }
}

void trippin::GameObject::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        syncChannel(engineTicks);
        return;
    }

    if (accelerateWhenGrounded) {
        if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
            acceleration.x = runningAcceleration;
            advanceFrame(engineTicks);
        } else {
            acceleration.x = 0;
        }
    } else {
        advanceFrame(engineTicks);
    }

    auto collision = roundedBox.collision(goggin->roundedBox);
    if (collision) {
        collisionTicks = engineTicks + collisionDuration;
        if (stompable && !stomped && collision.testTop() && goggin->maxFallingVelocityAbove(0.15)) {
            stomped = true;
            lane = -2; // no-collision plane
            velocity.y = -terminalVelocity.y / 3; // upward jolt
            scoreTicker->add(stompPoints);
            goggin->addPointCloud(stompPoints, engineTicks);
        }
    }

    syncChannel(engineTicks);
}

void trippin::GameObject::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    flashCycle = (flashCycle + 1) % 2;
    auto fr = ch.flash && flashCycle == 0 ? ch.frame + (sprite->getFrames() / 2) : ch.frame;
    sprite->render(ch.roundedPosition, fr, camera);
}

void trippin::GameObject::setActivation(const Activation *act) {
    activation = act;
}

void trippin::GameObject::syncChannel(Uint32 engineTicks) {
    auto flash = collisionTicks > engineTicks;
    channel.set({roundedPosition, frame, flash});
}

void trippin::GameObject::advanceFrame(Uint32 engineTicks) {
    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        frame = (frame + 1) % (sprite->getFrames() / 2);
    }
}
