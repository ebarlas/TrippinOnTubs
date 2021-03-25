#ifndef TRIPPIN_SCORETICKER_H
#define TRIPPIN_SCORETICKER_H

#include "engine/Listener.h"
#include "sprite/Sprite.h"
#include "Renderable.h"
#include "Configuration.h"
#include "Map.h"
#include "lock/Mutex.h"
#include "lock/Guarded.h"
#include "Goggin.h"

namespace trippin {
    class ScoreTicker : public Listener, public Renderable {
    private:
        const Sprite *digits{};
        int margin;
        double pointsPerTick;
        double score;
        const Goggin *goggin;

        Guarded<double> channel;
    public:
        // called from engine thread after tick
        void afterTick(Uint32 engineTicks) override;

        // called from engine thread on scoring collision to increase score
        void add(int n);

        // called from main render thread during initialization
        void setMargin(int margin);
        void setSprite(const Sprite &spr);
        void setPointsPerTick(double ppt);
        void setGoggin(const Goggin *goggin);
        void init();

        // called from main render thread
        void render(const Camera &camera) override;

        int getScore() const;
    };
}

#endif
