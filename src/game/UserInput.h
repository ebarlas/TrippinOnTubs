#ifndef TRIPPIN_USERINPUT_H
#define TRIPPIN_USERINPUT_H

#include "SDL.h"
#include "engine/Point.h"
#include "GogginInput.h"

namespace trippin {
    class UserInput {
    public:
        bool quitPressed() const;
        bool anythingPressed() const;
        bool pPressed() const;
        bool rPressed() const;
        Point<int> getLastPress() const;
        GogginInput asGogginInput() const;

        void pollEvents();
        void reset();
    private:
        struct Touch {
            bool touchDownEvent{};
            bool touchUpEvent{};
            bool touchHeldDown{};
            SDL_FingerID fingerId{};
        };

        struct Key {
            bool keyDownEvent{};
            bool keyUpEvent{};
            bool keyHeldDown{};
        };

        bool quit{};

        Key spaceKey;
        Key downKey;
        Key pKey;
        Key rKey;

        bool mouseButtonDownEvent{};
        Point<int> mouseButtonCoords{};

        Touch leftTouch;
        Touch rightTouch;

        void onMouseButtonDown(int x, int y);
        void onFingerDown(const SDL_TouchFingerEvent &event);
        void onFingerUp(const SDL_TouchFingerEvent &event);

        static void handleTouchDown(const SDL_TouchFingerEvent &e, Touch &touch);
        static void handleTouchUp(const SDL_TouchFingerEvent &e, Touch &touch);

        static void handleKeyDown(Key &key);
        static void handleKeyUp(Key &key);
    };
}

#endif
