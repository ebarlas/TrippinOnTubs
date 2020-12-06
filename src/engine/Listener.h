#ifndef TRIPPIN_LISTENER_H
#define TRIPPIN_LISTENER_H

#include "SDL.h"

namespace trippin {
    class Listener {
    public:
        virtual void beforeTick(Uint32 engineTicks) = 0;
        virtual void afterTick(Uint32 engineTicks) = 0;
    };
}

#endif