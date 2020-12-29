#ifndef TRIPPIN_SCORE_H
#define TRIPPIN_SCORE_H

#include "sprite/Sprite.h"
#include "Renderable.h"
#include "Configuration.h"
#include "Map.h"
#include "lock/Mutex.h"
#include "lock/Guarded.h"

namespace trippin {
    class Score : public Renderable {
    private:
        const Sprite *digits{};
        Point<int> topRight;

        struct Channel {
            int score;
        };
        Guarded<Channel> channel;
    public:
        // called from engine thread on scoring collision to increase score
        void add(int n);

        // called from main render thread during initialization
        void init(const Configuration &config, const Sprite &spr);

        // called from main render thread
        void render(const Camera &camera) override;
    };
}

#endif
