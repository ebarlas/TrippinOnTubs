#include "GameObject.h"
#include "engine/Convert.h"
#include "engine/Collisions.h"

void trippin::GameObject::setGoggin(Goggin &g) {
    goggin = &g;
}

void trippin::GameObject::setScoreTicker(ScoreTicker &st) {
    scoreTicker = &st;
}

void trippin::GameObject::setSoundManager(SoundManager &sm) {
    soundManager = &sm;
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
    objectActivation = obj.activation;
    stomped = false;
    if (obj.randFrame) {
        frame = std::rand() % spr.getFrames() / 2;
    } else {
        frame = obj.frame;
    }
    collisionDuration = config.ticksPerSecond() * 0.4;
    coolDownTicks = config.ticksPerSecond() * 0.15;
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
    stompSound = soundManager->getEffect("chime0");
    availableHitPoints = hitPoints = obj.hitPoints;
    healthBarSize = {
            toInt(config.healthBarSize.x * sprite->getScale().getMultiplier()),
            toInt(config.healthBarSize.y * sprite->getScale().getMultiplier())
    };
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

    if (stompable && !stomped && (engineTicks >= collisionTicks + coolDownTicks)) {
        auto collision = roundedBox.collision(goggin->roundedBox);
        if (collision) {
            if (collision.testTop()) {
                hitPoints = 0;
            } else {
                hitPoints -= 1;
            }
            collisionTicks = engineTicks;
        }

        if (hitPoints == 0) {
            stomped = true;
            lane = -2; // no-collision plane
            gravity = goggin->gravity;
            velocity.y = -terminalVelocity.y / 2; // upward jolt
            velocity.x = 0;
            scoreTicker->add(availableHitPoints * 25);
            goggin->addPointCloud(availableHitPoints * 25, engineTicks);
        }
    }

    syncChannel(engineTicks);
}

void trippin::GameObject::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (!ch.visible) {
        return;
    }

    flashCycle = (flashCycle + 1) % 2;
    auto fr = ch.flash && flashCycle == 0 ? ch.frame + (sprite->getFrames() / 2) : ch.frame;
    sprite->render(ch.roundedPosition, fr, camera);
    if (ch.collision) {
        if (!playedSound) {
            Mix_PlayChannel(-1, stompSound, 0);
            playedSound = true;
        }
    }
    if (ch.flash) {
        drawHealthBar(camera, ch.hitPoints);
    }
}

void trippin::GameObject::setActivation(const Activation *act) {
    activation = act;
}

void trippin::GameObject::syncChannel(Uint32 engineTicks) {
    auto flash = collisionTicks > 0 && collisionTicks + collisionDuration > engineTicks;
    channel.set({roundedPosition, frame, flash, stomped, !expired, hitPoints});
}

void trippin::GameObject::advanceFrame(Uint32 engineTicks) {
    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        frame = (frame + 1) % (sprite->getFrames() / 2);
    }
}

void trippin::GameObject::drawHealthBar(const trippin::Camera &camera, int hp) {
    auto vp = camera.getViewport();
    auto ren = sprite->getRenderer();
    auto percent = (double) hp / availableHitPoints;
    auto margin = healthBarSize.y * 3;
    auto x = roundedPosition.x - vp.x;
    auto y = roundedPosition.y - vp.y - margin;
    SDL_Rect outline{x, y, healthBarSize.x, healthBarSize.y};
    SDL_Rect fill{x, y, (int) (percent * healthBarSize.x), healthBarSize.y};
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 237, 76, 92, 100);
    SDL_RenderFillRect(ren, &fill);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 100);
    SDL_RenderDrawRect(ren, &outline);
}
