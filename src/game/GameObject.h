#ifndef TRIPPIN_GAMEOBJECT_H
#define TRIPPIN_GAMEOBJECT_H

#include "SpriteObject.h"
#include "Activation.h"
#include "ScoreTicker.h"
#include "lock/Guarded.h"

namespace trippin {
    class GameObject : public SpriteObject {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr) override;
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        void setActivation(const Activation *activation);
        void setGoggin(Goggin &goggin);
        void setScoreTicker(ScoreTicker &st);
        void setSoundManager(SoundManager &sm);
    private:
        double runningAcceleration;
        const Activation *activation;
        int frame;
        bool accelerateWhenGrounded;
        bool stompable;
        bool stomped;
        double objectActivation;
        Goggin *goggin;
        ScoreTicker *scoreTicker;

        int availableHitPoints;
        int hitPoints;
        Point<int> healthBarSize;

        SoundManager *soundManager;
        Mix_Chunk *stompSound;

        int collisionDuration;
        Uint32 collisionTicks;
        Uint32 coolDownTicks;

        struct Channel {
            Point<int> roundedPosition;
            int frame;
            bool flash;
            bool collision;
            bool visible;
            int hitPoints;
        };
        int flashCycle;
        bool playedSound;
        Guarded<Channel> channel;
        void syncChannel(Uint32 engineTicks);
        void advanceFrame(Uint32 engineTicks);

        void drawHealthBar(const trippin::Camera &camera, int hp);
    };
}

#endif
