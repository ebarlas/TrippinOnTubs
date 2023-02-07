#ifndef TRIPPIN_GAMEOBJECT_H
#define TRIPPIN_GAMEOBJECT_H

#include "SpriteObject.h"
#include "Activation.h"
#include "ScoreTicker.h"
#include "SceneBuilder.h"
#include "GroupManager.h"
#include "NotificationManager.h"

namespace trippin {
    class GameObject : public SpriteObject {
    public:
        GameObject(
                const Configuration &configuration,
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
                NotificationManager &groupNotificationManager);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
    private:
        const Map::Object &object;
        const Configuration &config;
        const Configuration::Object &configObject;
        Goggin &goggin;
        const Activation &activation;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        const Camera &camera;
        GroupManager &groupManager;
        NotificationManager &groupNotificationManager;

        Mix_Chunk *const stompSound;
        const int collisionDuration;
        const int coolDownTicks;
        const int flashDuration;

        int frame;
        bool stomped;
        int hitPoints;
        int collisionTicks;

        void drawSprite(int engineTicks);
        void drawHealthBar();
    };
}

#endif
