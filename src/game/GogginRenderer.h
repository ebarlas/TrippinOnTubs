#ifndef TRIPPIN_GOGGINRENDERER_H
#define TRIPPIN_GOGGINRENDERER_H

#include "Goggin.h"

namespace trippin {
    // Separate renderer class for Goggin game object
    // This simple class is required to allow for separation of (1) and (2) below
    // (1) goggin model updates including camera centering, which must occur first in each engine tick
    // (2) goggin rendering, which must occur last for goggin sprite to be laid atop other objects
    class GogginRenderer : public Listener {
    public:
        GogginRenderer(Goggin &goggin);
        void afterTick(int engineTicks) override;
        bool isExpired() override;
    private:
        Goggin &goggin;
    };
}

#endif
