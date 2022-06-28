#ifndef TRIPPIN_LISTENER_H
#define TRIPPIN_LISTENER_H

#include "SDL.h"

namespace trippin {
    class Listener {
    public:
        // Called from engine thread before tick processing begins in engine.
        virtual void beforeTick(int engineTicks);

        // Called from engine thread after tick processing ends in engine.
        virtual void afterTick(int engineTicks);

        // Called from engine thread during tick processing to determine whether listener should be removed.
        virtual bool isExpired();

        virtual ~Listener() = default;
    };
}

#endif