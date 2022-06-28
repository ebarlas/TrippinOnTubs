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
        const Rect<int> viewport;
        SceneBuilder &sceneBuilder;
    public:
        ScoreTicker(
                int margin,
                const Sprite &digits,
                int score,
                Rect<int> viewport,
                SceneBuilder &sceneBuilder);
        void add(int n);
        void afterTick(Uint32 engineTicks) override;
        int getScore() const;
    };
}

#endif
