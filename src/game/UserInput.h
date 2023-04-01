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

        UserInput(Point<int> rendererSize, int highDpiScale);
        Event pollEvent();
    private:
        struct Touch {
            bool touchHeldDown;
            SDL_FingerID fingerId;
        };

        const Point<int> rendererSize;

        // https://github.com/libsdl-org/SDL/issues/2999
        // https://forums.libsdl.org/viewtopic.php?p=43373
        const int highDpiScale;

        bool spaceKeyHeldDown{};
        bool downKeyHeldDown{};
        bool pKeyHeldDown{};
        bool rKeyHeldDown{};

        Touch leftTouch{};
        Touch rightTouch{};
    };
}

#endif
