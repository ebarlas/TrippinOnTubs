#ifndef TRIPPIN_GOGGININPUTTICK_H
#define TRIPPIN_GOGGININPUTTICK_H

#include "SDL.h"
#include "GogginInput.h"

namespace trippin {
    struct GogginInputTick : GogginInput {
        int tick{};
    };
}

#endif
