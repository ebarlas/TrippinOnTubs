#ifndef TRIPPIN_SCORETICKER_H
#define TRIPPIN_SCORETICKER_H

#include <atomic>
#include "engine/Listener.h"
#include "sprite/Sprite.h"
#include "SceneBuilder.h"

namespace trippin {
    class ScoreTicker : public Listener {
    private:
        const int margin;
        const Sprite &digits;
        std::atomic_int score;
        const Rect<int> windowSize;
        SceneBuilder &sceneBuilder;
        const int zIndex;
    public:
        ScoreTicker(
                int margin, // sprite scale
                const Sprite &digits,
                int score,
                Rect<int> windowSize, // sprite scale
                SceneBuilder &sceneBuilder,
                int zIndex);
        void add(int n);
        void afterTick(Uint32 engineTicks) override;
        int getScore() const;
    };
}

#endif
