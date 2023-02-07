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
        NotificationManager &groupNotificationManager) :
        SpriteObject(configObject, object, sprite),
        object(object),
        config(config),
        configObject(configObject),
        goggin(goggin),
        activation(activation),
        scoreTicker(scoreTicker),
        sceneBuilder(sceneBuilder),
        camera(camera),
        groupManager(groupManager),
        groupNotificationManager(groupNotificationManager),
        stompSound(soundManager.getEffect("chime0")),
        collisionDuration(static_cast<const int>(config.ticksPerSecond() * 0.4)),
        coolDownTicks(static_cast<const int>(config.ticksPerSecond() * 0.15)),
        flashDuration(static_cast<const int>(config.ticksPerSecond() * 0.025)) {
    inactive = true;
    if (!configObject.accelerateWhenGrounded) {
        acceleration.x = configObject.runningAcceleration;
    }
    stomped = false;
    frame = configObject.randFrame ? Random<>{}.next() % sprite.getFrames() / 2 : 0;
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
            auto points = config.pointsPerObject + config.pointsPerHitPoint * configObject.hitPoints;
            scoreTicker.add(points);
            goggin.addPointCloud(points, engineTicks, true);
            if (object.group) {
                groupManager.remove(object.group, id);
                if (groupManager.empty(object.group)) {
                    auto groupSize = groupManager.size(object.group);
                    goggin.addPointCloud(config.pointsPerGroup + groupSize * config.pointsPerMember, engineTicks);
                    groupNotificationManager.add(groupSize);
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
}

void trippin::GameObject::drawHealthBar() {
    auto vp = camera.getViewport();
    auto ren = sprite.getRenderer();
    auto percent = static_cast<double>(hitPoints) / configObject.hitPoints;
    auto pos = Point<int>{roundedPosition.x - vp.x, roundedPosition.y - vp.y};
    pos /= sprite.getScale().getDeviceEngineFactor();
    pos.y -= config.healthBarSize.y * 3;
    SDL_Rect outline{pos.x, pos.y, config.healthBarSize.x, config.healthBarSize.y};
    SDL_Rect fill{pos.x, pos.y, static_cast<int>(percent * config.healthBarSize.x), config.healthBarSize.y};
    sceneBuilder.dispatch([ren, fill, outline]() {
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(ren, 150, 150, 150, 100);
        SDL_RenderFillRect(ren, &outline);
        SDL_SetRenderDrawColor(ren, 237, 76, 92, 100);
        SDL_RenderFillRect(ren, &fill);
    });
}