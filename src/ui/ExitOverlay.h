#ifndef TRIPPIN_EXITOVERLAY_H
#define TRIPPIN_EXITOVERLAY_H

#include "sprite/Sprite.h"
#include "ui/Interpolator.h"

namespace trippin {
    class ExitOverlay {
    public:
        ExitOverlay(
                const Point<int> &windowSize,
                int margin,
                const Sprite &sprite,
                const RenderClock &renderClock,
                int slideDuration,
                int pauseDuration = 0);
        void reset();
        void render();
        bool exitClicked(const Point<int> &coords) const;
    private:
        enum class Direction {
            in,
            pause,
            out
        };

        const Sprite &sprite;
        const int slideDuration;
        const int pauseDuration;
        const int top;
        Direction direction;
        Interpolator inInterpolator;
        Interpolator outInterpolator;

        int interpolate() const;
    };
}

#endif
