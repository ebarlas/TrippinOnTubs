#ifndef TRIPPIN_GAMEOBJECT_H
#define TRIPPIN_GAMEOBJECT_H

#include "game/SpriteObject.h"
#include "game/Activation.h"
#include "game/ScoreTicker.h"
#include "game/SceneBuilder.h"
#include "game/Goggin.h"

namespace trippin {
    class GameObject : public SpriteObject {
    public:
        GameObject(
                const Configuration &config,
                const Map::Object &object, // engine scale
                const Sprite &sprite,
                Goggin &goggin,
                const Activation &activation,
                ScoreTicker &scoreTicker,
                SoundManager &soundManager,
                const Camera &camera,
                SceneBuilder &sceneBuilder,
                int zIndex,
                const Units &units);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    private:
        const Map::Object &object;
        Goggin &goggin;
        const Activation &activation;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        const Camera &camera;
        const int zIndex;
        const int collisionDuration;

        const Uint32 coolDownTicks;
        const Uint32 flashDuration;
        Mix_Chunk *const stompSound;
        const Point<int> healthBarSize; // sprite scale
        const Units &units;

        int frame;
        bool stomped;
        int hitPoints;
        Uint32 collisionTicks;

        void advanceFrame(Uint32 engineTicks);
        void drawSprite(Uint32 engineTicks);
        void drawHealthBar();
    };
}

#endif
