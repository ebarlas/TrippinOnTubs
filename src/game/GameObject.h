#ifndef TRIPPIN_GAMEOBJECT_H
#define TRIPPIN_GAMEOBJECT_H

#include "SpriteObject.h"
#include "Activation.h"
#include "ScoreTicker.h"
#include "SceneBuilder.h"

namespace trippin {
    class GameObject : public SpriteObject {
    public:
        GameObject(
                const Configuration &config,
                const Map::Object &object,
                const Sprite &sprite,
                Goggin &goggin,
                const Activation &activation,
                ScoreTicker &scoreTicker,
                SoundManager &soundManager,
                const Camera &camera,
                SceneBuilder &sceneBuilder,
                int zIndex);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    private:
        Goggin &goggin;
        const Activation &activation;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        const Camera &camera;
        const int zIndex;

        const double runningAcceleration;
        const bool accelerateWhenGrounded;
        Mix_Chunk *const stompSound;
        const bool stompable;
        const bool topStompable;
        const bool bottomStompable;
        const double objectActivation;
        const int collisionDuration;
        const Uint32 coolDownTicks;
        const Uint32 flashDuration;
        const int availableHitPoints;
        const Point<int> healthBarSize;

        int frame;
        bool stomped;
        int hitPoints;
        Uint32 collisionTicks;

        void advanceFrame(Uint32 engineTicks);
        void drawSprite(Uint32 engineTicks);
        void drawHealthBar();
        static Point<int> scaleHealthBar(Point<int> healthBarSize, const Sprite &sprite);
    };
}

#endif
