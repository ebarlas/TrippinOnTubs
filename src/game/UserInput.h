#ifndef TRIPPIN_USERINPUT_H
#define TRIPPIN_USERINPUT_H

#include "SDL.h"
#include "engine/Point.h"
#include "GogginInput.h"

namespace trippin {
    class UserInput {
    public:
        struct Event {
            bool render;
            bool quit;
            bool spaceKeyDown;
            bool spaceKeyUp;
            bool downKeyDown;
            bool downKeyUp;
            bool pKeyDown;
            bool pKeyUp;
            bool rKeyDown;
            bool rKeyUp;
            bool mouseButtonDown;
            bool leftTouchDown;
            bool leftTouchUp;
            bool rightTouchDown;
            bool rightTouchUp;
            bool focusLost;
            bool focusGained;
            Point<int> touchPoint;

            GogginInput asGogginInput() const;
            bool anythingPressed() const;
            operator bool() const;
        };

        Event pollEvent();
    private:
        struct Touch {
            bool touchHeldDown;
            SDL_FingerID fingerId;
        };

        bool spaceKeyHeldDown{};
        bool downKeyHeldDown{};
        bool pKeyHeldDown{};
        bool rKeyHeldDown{};

        Touch leftTouch{};
        Touch rightTouch{};
    };
}

#endif
