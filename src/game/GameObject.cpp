#include "GameObject.h"
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
        int zIndex,
        const Units &units) :
        SpriteObject(object, sprite, units),
        object(object),
        goggin(goggin),
        activation(activation),
        scoreTicker(scoreTicker),
        sceneBuilder(sceneBuilder),
        camera(camera),
        zIndex(zIndex),
        collisionDuration(config.ticksPerSecond() * 0.4),
        coolDownTicks(config.ticksPerSecond() * 0.15),
        flashDuration(config.ticksPerSecond() * 0.025),
        stompSound(soundManager.getEffect("chime0")),
        healthBarSize(units.baseToSprite(config.healthBarSize)),
        units(units) {
    inactive = true;
    if (!object.accelerateWhenGrounded) {
        acceleration.x = object.runningAcceleration;
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
            onReflectiveCollision(left, right, sides, {coefficient, 100});
        });
    }
    if (object.elasticObjectCollisions) {
        objectCollision.set(onElasticCollision2D);
    }
    hitPoints = object.hitPoints;
}

void trippin::GameObject::beforeTick(Uint32 engineTicks) {
    if (inactive) {
        if (object.activation > 0) {
            if (goggin.position.x >= position.x - object.activation) {
                inactive = false;
            }
        } else {
            if (activation.shouldActivate(box)) {
                inactive = false;
            }
        }
    }
}

void trippin::GameObject::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        // hack to ensure "stalled" objects are drawn
        if (object.activation > 0) {
            drawSprite(engineTicks);
        }
        return;
    }

    if (activation.shouldDeactivate(box)) {
        expired = true;
        return;
    }

    if (object.accelerateWhenGrounded) {
        if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
            acceleration.x = object.runningAcceleration;
            advanceFrame(engineTicks);
        } else {
            acceleration.x = 0;
        }
    } else {
        advanceFrame(engineTicks);
    }

    if (!stomped && (engineTicks >= collisionTicks + coolDownTicks)) {
        auto collision = box.collision(goggin.box);
        if (collision) {
            if (collision.testTop()) {
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
            scoreTicker.add(object.hitPoints * 25);
            goggin.addPointCloud(object.hitPoints * 25, engineTicks, true);
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
    auto posNow = position;
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
    auto percent = (double) hitPoints / object.hitPoints;
    auto pos = units.engineToSprite(Point<int_fast64_t>{position.x - vp.x, position.y - vp.y});
    pos.y -= healthBarSize.y * 3;
    SDL_Rect outline{pos.x, pos.y, healthBarSize.x, healthBarSize.y};
    SDL_Rect fill{pos.x, pos.y, static_cast<int>(percent * healthBarSize.x), healthBarSize.y};
    sceneBuilder.dispatch([ren, fill, outline]() {
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(ren, 150, 150, 150, 100);
        SDL_RenderFillRect(ren, &outline);
        SDL_SetRenderDrawColor(ren, 237, 76, 92, 100);
        SDL_RenderFillRect(ren, &fill);
    });
}