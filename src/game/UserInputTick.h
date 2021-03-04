#ifndef TRIPPIN_USERINPUTTICK_H
#define TRIPPIN_USERINPUTTICK_H

#include "SDL.h"
#include "UserInput.h"

namespace trippin {
    struct UserInputTick : UserInput {
        Uint32 tick{};
    };
}

#endif
