#include "UserInput.h"
#include "engine/Convert.h"

bool trippin::UserInput::quitPressed() const {
    return quit;
}

bool trippin::UserInput::anythingPressed() const {
    return mouseButtonDownEvent
           || spaceKey.keyHeldDown
           || downKey.keyHeldDown
           || leftTouch.touchDownEvent
           || rightTouch.touchDownEvent;
}

trippin::Point<int> trippin::UserInput::getLastPress() const {
    if (mouseButtonDownEvent) {
        return mouseButtonCoords;
    }
    return {};
}

trippin::GogginInput trippin::UserInput::asGogginInput() const {
    GogginInput gi;
    if (spaceKey.keyDownEvent || rightTouch.touchDownEvent) {
        gi.jumpCharge = true;
    }
    if (spaceKey.keyUpEvent || rightTouch.touchUpEvent) {
        gi.jumpRelease = true;
    }
    if (downKey.keyDownEvent || leftTouch.touchDownEvent) {
        gi.duckStart = true;
    }
    if (downKey.keyUpEvent || leftTouch.touchUpEvent) {
        gi.duckEnd = true;
    }
    return gi;
}

void trippin::UserInput::pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                handleKeyDown(spaceKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                handleKeyDown(downKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_P) {
                handleKeyDown(pKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_R) {
                handleKeyDown(rKey);
            }
        } else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                handleKeyUp(spaceKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                handleKeyUp(downKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_P) {
                handleKeyUp(pKey);
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_R) {
                handleKeyUp(rKey);
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            onMouseButtonDown(e.button.x, e.button.y);
        } else if (e.type == SDL_FINGERDOWN) {
            onFingerDown(e.tfinger);
        } else if (e.type == SDL_FINGERUP) {
            onFingerUp(e.tfinger);
        } else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                focusLost = true;
            } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                focusGained = true;
            }
        }
    }
}

void trippin::UserInput::reset() {
    mouseButtonDownEvent = false;
    spaceKey.keyDownEvent = spaceKey.keyUpEvent = false;
    downKey.keyDownEvent = downKey.keyUpEvent = false;
    pKey.keyDownEvent = pKey.keyUpEvent = false;
    rKey.keyDownEvent = rKey.keyUpEvent = false;
    leftTouch.touchDownEvent = leftTouch.touchUpEvent = false;
    rightTouch.touchDownEvent = rightTouch.touchUpEvent = false;
    focusLost = focusGained = false;
}

void trippin::UserInput::onMouseButtonDown(int x, int y) {
    mouseButtonDownEvent = true;
    mouseButtonCoords = {x, y};
}

void trippin::UserInput::onFingerDown(const SDL_TouchFingerEvent &e) {
    auto &touch = e.x < 0.5 ? leftTouch : rightTouch;
    handleTouchDown(e, touch);
}

void trippin::UserInput::onFingerUp(const SDL_TouchFingerEvent &e) {
    handleTouchUp(e, leftTouch);
    handleTouchUp(e, rightTouch);
}

void trippin::UserInput::handleTouchDown(const SDL_TouchFingerEvent &e, Touch &touch) {
    if (!touch.touchHeldDown) {
        touch.touchHeldDown = true;
        touch.touchDownEvent = true;
        touch.touchUpEvent = false;
        touch.fingerId = e.fingerId;
    }
}

void trippin::UserInput::handleTouchUp(const SDL_TouchFingerEvent &e, Touch &touch) {
    if (touch.touchHeldDown && touch.fingerId == e.fingerId) {
        touch.touchHeldDown = false;
        touch.touchDownEvent = false;
        touch.touchUpEvent = true;
    }
}

void trippin::UserInput::handleKeyDown(Key &key) {
    if (!key.keyHeldDown) {
        key.keyHeldDown = true;
        key.keyDownEvent = true;
        key.keyUpEvent = false;
    }
}

void trippin::UserInput::handleKeyUp(Key &key) {
    if (key.keyHeldDown) {
        key.keyHeldDown = false;
        key.keyDownEvent = false;
        key.keyUpEvent = true;
    }
}

bool trippin::UserInput::pPressed() const {
    return pKey.keyDownEvent;
}

bool trippin::UserInput::rPressed() const {
    return rKey.keyDownEvent;
}

bool trippin::UserInput::wasFocusLost() const {
    return focusLost;
}

bool trippin::UserInput::wasFocusGained() const {
    return focusGained;
}
