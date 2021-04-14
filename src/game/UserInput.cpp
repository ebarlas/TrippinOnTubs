#include "UserInput.h"
#include "engine/Convert.h"

trippin::UserInput::UserInput(Point<int> windowSize) : windowSize(windowSize) {

}

bool trippin::UserInput::quitPressed() const {
    return quit;
}

bool trippin::UserInput::anythingPressed() const {
    return mouseButtonDownEvent
           || spaceKeyHeldDown
           || downKeyHeldDown
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
    if (spaceKeyDownEvent || rightTouch.touchDownEvent) {
        gi.jumpCharge = true;
    }
    if (spaceKeyUpEvent || rightTouch.touchUpEvent) {
        gi.jumpRelease = true;
    }
    if (downKeyDownEvent || leftTouch.touchDownEvent) {
        gi.duckStart = true;
    }
    if (downKeyUpEvent || leftTouch.touchUpEvent) {
        gi.duckEnd = true;
    }
    return gi;
}

void trippin::UserInput::pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            onQuit();
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                onSpaceKeyDown();
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                onDownKeyDown();
            }
        } else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                onSpaceKeyUp();
            }
            if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                onDownKeyUp();
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            onMouseButtonDown(e.button.x, e.button.y);
        } else if (e.type == SDL_FINGERDOWN) {
            onFingerDown(e.tfinger);
        } else if (e.type == SDL_FINGERUP) {
            onFingerUp(e.tfinger);
        }
    }
}

void trippin::UserInput::reset() {
    spaceKeyDownEvent = spaceKeyUpEvent = downKeyDownEvent = downKeyUpEvent = mouseButtonDownEvent = false;
    leftTouch.touchDownEvent = leftTouch.touchUpEvent = rightTouch.touchDownEvent = rightTouch.touchUpEvent = false;
}

void trippin::UserInput::onQuit() {
    quit = true;
}

void trippin::UserInput::onSpaceKeyDown() {
    if (!spaceKeyHeldDown) {
        spaceKeyHeldDown = true;
        spaceKeyDownEvent = true;
        spaceKeyUpEvent = false;
    }
}

void trippin::UserInput::onSpaceKeyUp() {
    if (spaceKeyHeldDown) {
        spaceKeyHeldDown = false;
        spaceKeyDownEvent = false;
        spaceKeyUpEvent = true;
    }
}

void trippin::UserInput::onDownKeyDown() {
    if (!downKeyHeldDown) {
        downKeyDownEvent = true;
        downKeyHeldDown = true;
        downKeyUpEvent = false;
    }
}

void trippin::UserInput::onDownKeyUp() {
    if (downKeyHeldDown) {
        downKeyHeldDown = false;
        downKeyDownEvent = false;
        downKeyUpEvent = true;
    }
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
