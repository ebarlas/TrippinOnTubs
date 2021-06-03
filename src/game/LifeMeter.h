#ifndef TRIPPIN_LIFEMETER_H
#define TRIPPIN_LIFEMETER_H

#include "sprite/Sprite.h"
#include "Renderable.h"

namespace trippin {
    class LifeMeter : public Renderable {
    public:
        LifeMeter(int margin, const Sprite &sprite);
        void setExtraLives(int lives);
        void render(const Camera &camera) override;
    private:
        const Sprite &sprite;
        const int margin;
        int extraLives;
    };
}

#endif
