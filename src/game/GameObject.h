#ifndef TRIPPIN_GAMEOBJECT_H
#define TRIPPIN_GAMEOBJECT_H

#include "SpriteObject.h"
#include "Activation.h"
#include "ScoreTicker.h"
#include "SceneBuilder.h"
#include "GroupManager.h"

namespace trippin {
    class GameObject : public SpriteObject {
    public:
        GameObject(
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
                const Sprite &sparkleSprite);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
    private:
        const Map::Object &object;
        const Configuration::Object &configObject;
        Goggin &goggin;
        const Activation &activation;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        const Camera &camera;
        GroupManager &groupManager;
        const Sprite &sparkleSprite;

        Mix_Chunk *const stompSound;
        const int collisionDuration;
        const int coolDownTicks;
        const int flashDuration;
        const Point<int> healthBarSize;

        int frame;
        bool stomped;
        int hitPoints;
        int collisionTicks;
        int sparkleFrame;

        void drawSprite(int engineTicks);
        void drawHealthBar();
    };
}

#endif
