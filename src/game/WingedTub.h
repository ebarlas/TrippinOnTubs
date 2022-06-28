#ifndef TRIPPIN_WINGEDTUB_H
#define TRIPPIN_WINGEDTUB_H

#include "engine/Listener.h"
#include "Spirit.h"
#include "Goggin.h"
#include "ScoreTicker.h"
#include "Activation.h"

namespace trippin {
    class WingedTub : public Listener {
    public:
        WingedTub(
                const Map::Object &object,
                const Sprite &sprite,
                const Activation &activation,
                Goggin &goggin,
                ScoreTicker &scoreTicker,
                SoundManager &soundManager,
                const Camera &camera,
                SceneBuilder &sceneBuilder);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
        bool isExpired() override;
    private:
        constexpr static const int FRAME_CLOUD_FIRST = 10;
        constexpr static const int FRAME_CLOUD_LAST = 19;
        constexpr static const int FRAME_TUB_FIRST = 0;
        constexpr static const int FRAME_TUB_LAST = 9;
        constexpr static const int FRAME_SPARKLE_FIRST = 20;
        constexpr static const int FRAME_SPARKLE_LAST = 29;

        const Sprite &sprite;
        const Activation &activation;
        Goggin &goggin;
        ScoreTicker &scoreTicker;
        SceneBuilder &sceneBuilder;
        const Camera &camera;

        const Point<int> position;
        const Rect<int> hitBox;
        const int tubFrameFirst;
        const int tubFrameLast;
        const int points;
        Mix_Chunk *const sound;

        int hitTicks;
        bool hitGoggin;
        bool expired;
        bool inactive;
        int frame;
    };
}

#endif
