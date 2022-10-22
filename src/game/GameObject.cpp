#include "GameObject.h"
#include "engine/Collisions.h"
#include "Random.h"

trippin::GameObject::GameObject(
        const Configuration &config,
        const Configuration::Object &configObject,
        const Map::Object &object,
        const Sprite &sprite,
        Goggin &goggin,
        const Activation &activation,
        ScoreTicker &scoreTicker,
        SoundManager &soundManager,
        const Camera &camera,
        SceneBuilder &sceneBuilder,
        GroupManager &groupManager,
        const Sprite &haloSprite) :
        SpriteObject(configObject, object, sprite),
        object(object),
        configObject(configObject),
        goggin(goggin),
        activation(activation),
        scoreTicker(scoreTicker),
        sceneBuilder(sceneBuilder),
        camera(camera),
        groupManager(groupManager),
        haloSprite(haloSprite),
        stompSound(soundManager.getEffect("chime0")),
        collisionDuration(static_cast<const int>(config.ticksPerSecond() * 0.4)),
        coolDownTicks(static_cast<const int>(config.ticksPerSecond() * 0.15)),
        flashDuration(static_cast<const int>(config.ticksPerSecond() * 0.025)),
        healthBarSize(config.healthBarSize) {
    inactive = true;
    if (!configObject.accelerateWhenGrounded) {
        acceleration.x = configObject.runningAcceleration;
    }
    stomped = false;
    frame = configObject.randFrame ? Random<>{}.next() % sprite.getFrames() / 2 : 0;
    haloFrame = Random<>{}.next() % haloSprite.getFrames();
    collisionTicks = 0;
    if (configObject.coefficient > 0) {
        auto coefficient = configObject.coefficient;
        platformCollision.set([coefficient](Object &left, Object &right, const Sides &sides) {
            onReflectiveCollision(left, right, sides, coefficient);
        });
    }
    if (configObject.elasticObjectCollisions) {
        objectCollision.set(onElasticCollision2D);
    }
    hitPoints = configObject.hitPoints;
}

void trippin::GameObject::beforeTick(int) {
    if (inactive) {
        if (object.activation > 0) {
            if (goggin.position.x >= position.x - object.activation) {
                inactive = false;
            }
        } else {
            if (activation.shouldActivate(roundedBox)) {
                inactive = false;
            }
        }
    }
}

void trippin::GameObject::afterTick(int engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        // hack to ensure "stalled" objects are drawn
        if (object.activation > 0) {
            drawSprite(engineTicks);
        }
        return;
    }

    if (activation.shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }

    if (object.group) {
        haloSprite.advanceFrame(engineTicks, haloFrame);
    }

    if (configObject.accelerateWhenGrounded) {
        if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
            acceleration.x = configObject.runningAcceleration;
            sprite.advanceFrame(engineTicks, frame, true);
        } else {
            acceleration.x = 0;
        }
    } else {
        sprite.advanceFrame(engineTicks, frame, true);
    }

    if (!stomped && (engineTicks >= collisionTicks + coolDownTicks)) {
        auto collision = roundedBox.collision(goggin.roundedBox);
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
            scoreTicker.add(configObject.hitPoints * 25);
            goggin.addPointCloud(configObject.hitPoints * 25, engineTicks, true);
            if (object.group) {
                groupManager.remove(object.group, id);
                if (groupManager.empty(object.group)) {
                    goggin.addPointCloud(groupManager.size(object.group) * 25, engineTicks);
                }
            }
        }
    }

    drawSprite(engineTicks);
}

void trippin::GameObject::drawSprite(int engineTicks) {
    int frameNow = frame;
    auto posNow = roundedPosition;
    auto flash = collisionTicks > 0 && collisionTicks + collisionDuration > engineTicks;
    if (flash) {
        auto offset = engineTicks - collisionTicks;
        frameNow = offset / flashDuration % 2 == 0 ? frame + (sprite.getFrames() / 2) : frame;
        drawHealthBar();
    }

    auto vp = camera.getViewport();
    sceneBuilder.dispatch([this, posNow, frameNow, vp]() {
        sprite.renderEngine(posNow, frameNow, vp);
    });

    if (object.group && !stomped) {
        auto [_x, _y, width, height] = haloSprite.getEngineHitBox();
        auto widthDiff = size.x - width;
        auto x = roundedPosition.x + widthDiff / 2;
        auto y = roundedPosition.y - height;
        auto haloPos = Point<int>{x, y};
        auto rescale = 0.4 + (1.0 - groupManager.remaining(object.group)) * 0.6;
        auto fr = haloFrame;
        sceneBuilder.dispatch([this, haloPos, fr, rescale, vp]() {
            haloSprite.renderEngine(haloPos, fr, vp, rescale);
        });
    }
}

void trippin::GameObject::drawHealthBar() {
    auto vp = camera.getViewport();
    auto ren = sprite.getRenderer();
    auto percent = static_cast<double>(hitPoints) / configObject.hitPoints;
    auto pos = Point<int>{roundedPosition.x - vp.x, roundedPosition.y - vp.y};
    pos /= sprite.getScale().getDeviceEngineFactor();
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