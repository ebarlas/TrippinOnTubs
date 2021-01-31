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
        int margin;

        struct Channel {
            int score;
        };
        Guarded<Channel> channel;
    public:
        // called from engine thread on scoring collision to increase score
        void add(int n);

        // called from main render thread during initialization
        void setMargin(int margin);
        void setSprite(const Sprite &spr);
        void init();

        // called from main render thread
        void render(const Camera &camera) override;
    };
}

#endif
