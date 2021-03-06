#ifndef TRIPPIN_WINGEDTUB_H
#define TRIPPIN_WINGEDTUB_H

#include "engine/Listener.h"
#include "Spirit.h"
#include "Goggin.h"
#include "ScoreTicker.h"
#include "Activation.h"
#include "lock/Guarded.h"

namespace trippin {
    class WingedTub : public Renderable, public Listener {
    public:
        void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
        void setGoggin(Goggin *goggin);
        void setScoreTicker(ScoreTicker *scoreTicker);
        void setActivation(const Activation *activation);
        void setSoundManager(SoundManager &soundManager);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        void render(const Camera &camera) override;
        bool isExpired() override;
    private:
        constexpr static const int FRAME_CLOUD_FIRST = 10;
        constexpr static const int FRAME_CLOUD_LAST = 19;
        constexpr static const int FRAME_TUB_FIRST = 0;
        constexpr static const int FRAME_TUB_LAST = 9;
        constexpr static const int FRAME_SPARKLE_FIRST = 20;
        constexpr static const int FRAME_SPARKLE_LAST = 29;

        const Sprite *sprite;
        Goggin *goggin;
        ScoreTicker *scoreTicker;
        Point<int> position;
        Rect<int> hitBox;
        int hitTicks;
        bool hitGoggin;
        bool expired;
        bool inactive;
        const Activation *activation;
        SoundManager *soundManager;

        int points;
        int tubFrameFirst;
        int tubFrameLast;
        int frame;

        Mix_Chunk *sound;
        bool playedSound;

        struct Channel {
            int frame;
            bool visible;
        };

        Guarded<Channel> channel;
        void syncChannel();
    };
}

#endif
