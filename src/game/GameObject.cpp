#include "GameObject.h"
#include "engine/Convert.h"
#include "engine/Collisions.h"

trippin::GameObject::GameObject(
        const Configuration &config,
        const Map::Object &object,
        const Sprite &sprite,
        Goggin &goggin,
        const Activation &activation,
        ScoreTicker &scoreTicker,
        SoundManager &soundManager,
        const Camera &camera,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        SpriteObject(config, object, sprite),
        goggin(goggin),
        activation(activation),
        scoreTicker(scoreTicker),
        camera(camera),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        accelerateWhenGrounded(object.accelerateWhenGrounded),
        runningAcceleration(object.runningAcceleration),
        stompable(object.stompable),
        topStompable(object.topStompable),
        bottomStompable(object.bottomStompable),
        objectActivation(object.activation),
        collisionDuration(config.ticksPerSecond() * 0.4),
        coolDownTicks(config.ticksPerSecond() * 0.15),
        flashDuration(config.ticksPerSecond() * 0.025),
        stompSound(soundManager.getEffect("chime0")),
        availableHitPoints(object.hitPoints),
        healthBarSize(scaleHealthBar(config.healthBarSize, sprite)) {
    inactive = true;
    if (!accelerateWhenGrounded) {
        acceleration.x = runningAcceleration;
    }
    stomped = false;
    if (object.randFrame) {
        frame = std::rand() % sprite.getFrames() / 2;
    } else {
        frame = object.frame;
    }
    collisionTicks = 0;
    if (object.coefficient > 0) {
        auto coefficient = object.coefficient;
        platformCollision.set([coefficient](Object &left, Object &right, const Sides &sides) {
            onReflectiveCollision(left, right, sides, coefficient);
        });
    }
    if (object.elasticObjectCollisions) {
        objectCollision.set(onElasticCollision2D);
    }
    hitPoints = object.hitPoints;
}

void trippin::GameObject::beforeTick(Uint32 engineTicks) {
    if (inactive) {
        if (objectActivation > 0) {
            if (goggin.position.x >= position.x - objectActivation) {
                inactive = false;
            }
        } else {
            if (activation.shouldActivate(roundedBox)) {
                inactive = false;
            }
        }
    }
}

void trippin::GameObject::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        // hack to ensure "stalled" objects are drawn
        if (objectActivation > 0) {
            drawSprite(engineTicks);
        }
        return;
    }

    if (activation.shouldDeactivate(roundedBox)) {
        expired = true;
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
        auto collision = roundedBox.collision(goggin.roundedBox);
        if (collision) {
            if ((topStompable && collision.testTop()) || (bottomStompable && collision.testBottom())) {
                hitPoints = 0;
            } else {
                hitPoints -= 1;
            }
            collisionTicks = engineTicks;
        }

        if (hitPoints == 0) {
            Mix_PlayChannel(-1, stompSound, 0);
            stomped = true;
            lane = -2; // no-collision plane
            gravity = goggin.gravity;
            velocity.y = -terminalVelocity.y / 2; // upward jolt
            velocity.x = 0;
            scoreTicker.add(availableHitPoints * 25);
            goggin.addPointCloud(availableHitPoints * 25, engineTicks);
        }
    }

    drawSprite(engineTicks);
}

void trippin::GameObject::advanceFrame(Uint32 engineTicks) {
    if (engineTicks % sprite.getFramePeriodTicks() == 0) {
        frame = (frame + 1) % (sprite.getFrames() / 2);
    }
}

void trippin::GameObject::drawSprite(Uint32 engineTicks) {
    int frameNow = frame;
    auto posNow = roundedPosition;
    auto flash = collisionTicks > 0 && collisionTicks + collisionDuration > engineTicks;
    if (flash) {
        auto offset = engineTicks - collisionTicks;
        frameNow = offset / flashDuration % 2 == 0 ? frame + (sprite.getFrames() / 2) : frame;
        drawHealthBar();
    }

    sceneBuilder.dispatch([this, posNow, frameNow]() {
        sprite.render(posNow, frameNow, camera);
    }, zIndex);
}

void trippin::GameObject::drawHealthBar() {
    auto vp = camera.getViewport();
    auto ren = sprite.getRenderer();
    auto percent = (double) hitPoints / availableHitPoints;
    auto margin = healthBarSize.y * 3;
    auto x = roundedPosition.x - vp.x;
    auto y = roundedPosition.y - vp.y - margin;
    SDL_Rect outline{x, y, healthBarSize.x, healthBarSize.y};
    SDL_Rect fill{x, y, (int) (percent * healthBarSize.x), healthBarSize.y};
    sceneBuilder.dispatch([ren, fill, outline]() {
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(ren, 150, 150, 150, 100);
        SDL_RenderFillRect(ren, &outline);
        SDL_SetRenderDrawColor(ren, 237, 76, 92, 100);
        SDL_RenderFillRect(ren, &fill);
    });
}

trippin::Point<int> trippin::GameObject::scaleHealthBar(Point<int> healthBarSize, const Sprite &sprite) {
    auto mul = sprite.getScale().getMultiplier();
    return {toInt(healthBarSize.x * mul),toInt(healthBarSize.y * mul)};
}
