#ifndef TRIPPIN_USERINPUT_H
#define TRIPPIN_USERINPUT_H

#include "SDL.h"
#include "engine/Point.h"
#include "GogginInput.h"

namespace trippin {
    class UserInput {
    public:
        UserInput(Point<int> windowSize);
        bool quitPressed() const;
        bool anythingPressed() const;
        Point<int> getLastPress() const;
        GogginInput asGogginInput() const;

        void pollEvents();
        void reset();
    private:
        struct Touch {
            bool touchDownEvent{};
            bool touchUpEvent{};
            bool touchHeldDown{};
            SDL_FingerID fingerId;
        };

        Point<int> windowSize;

        bool quit{};

        bool spaceKeyDownEvent{};
        bool spaceKeyUpEvent{};
        bool spaceKeyHeldDown{};

        bool downKeyDownEvent{};
        bool downKeyUpEvent{};
        bool downKeyHeldDown{};

        bool mouseButtonDownEvent{};
        Point<int> mouseButtonCoords{};

        Touch leftTouch;
        Touch rightTouch;

        void onQuit();
        void onSpaceKeyDown();
        void onSpaceKeyUp();
        void onDownKeyDown();
        void onDownKeyUp();
        void onMouseButtonDown(int x, int y);
        void onFingerDown(const SDL_TouchFingerEvent &event);
        void onFingerUp(const SDL_TouchFingerEvent &event);

        void handleTouchDown(const SDL_TouchFingerEvent &e, Touch &touch);
        void handleTouchUp(const SDL_TouchFingerEvent &e, Touch &touch);
    };
}

#endif
