#include "UserInput.h"
#include "engine/Convert.h"

trippin::UserInput::UserInput(trippin::Point<int> rendererSize) : rendererSize(rendererSize) {
}

trippin::GogginInput trippin::UserInput::Event::asGogginInput() const {
    GogginInput gi;
    if (spaceKeyDown || rightTouchDown) {
        gi.jumpCharge = true;
    }
    if (spaceKeyUp || rightTouchUp) {
        gi.jumpRelease = true;
    }
    if (downKeyDown || leftTouchDown) {
        gi.duckStart = true;
    }
    if (downKeyUp || leftTouchUp) {
        gi.duckEnd = true;
    }
    return gi;
}

bool trippin::UserInput::Event::anythingPressed() const {
    return mouseButtonDown || spaceKeyDown || downKeyDown || leftTouchDown || rightTouchDown;
}

trippin::UserInput::Event::operator bool() const {
    return render || quit || spaceKeyDown || spaceKeyUp || downKeyDown || downKeyUp || pKeyDown || pKeyUp
           || rKeyDown || rKeyUp || mouseButtonDown || leftTouchDown || leftTouchUp || rightTouchDown || rightTouchUp
           || focusLost || focusGained;
}

trippin::UserInput::Event trippin::UserInput::pollEvent() {
    SDL_Event e;
    auto events = SDL_PollEvent(&e);

    Event result{};

    auto keyDownFn = [&e](int scanCode, bool &heldDown, bool &event) {
        if (e.key.keysym.scancode == scanCode) {
            if (!heldDown) {
                heldDown = true;
                event = true;
            }
        }
    };

    auto keyUpFn = [&e](int scanCode, bool &heldDown, bool &event) {
        if (e.key.keysym.scancode == scanCode) {
            if (heldDown) {
                heldDown = false;
                event = true;
            }
        }
    };

    auto touchDownFn = [&e](Touch &touch, bool &event) {
        if (!touch.touchHeldDown) {
            touch.touchHeldDown = true;
            touch.fingerId = e.tfinger.fingerId;
            event = true;
        }
    };

    auto touchUpFn = [&e](Touch &touch, bool &event) {
        if (touch.touchHeldDown && touch.fingerId == e.tfinger.fingerId) {
            touch.touchHeldDown = false;
            event = true;
        }
    };

    // with VSYNC enabled, returns 0 at sync interval (for ex, 60 per sec)
    if (events == 0) {
        result.render = true;
    } else if (e.type == SDL_QUIT) {
        result.quit = true;
    } else if (e.type == SDL_KEYDOWN) {
        keyDownFn(SDL_SCANCODE_SPACE, spaceKeyHeldDown, result.spaceKeyDown);
        keyDownFn(SDL_SCANCODE_DOWN, downKeyHeldDown, result.downKeyDown);
        keyDownFn(SDL_SCANCODE_P, pKeyHeldDown, result.pKeyDown);
        keyDownFn(SDL_SCANCODE_R, rKeyHeldDown, result.rKeyDown);
    } else if (e.type == SDL_KEYUP) {
        keyUpFn(SDL_SCANCODE_SPACE, spaceKeyHeldDown, result.spaceKeyUp);
        keyUpFn(SDL_SCANCODE_DOWN, downKeyHeldDown, result.downKeyUp);
        keyUpFn(SDL_SCANCODE_P, pKeyHeldDown, result.pKeyUp);
        keyUpFn(SDL_SCANCODE_R, rKeyHeldDown, result.rKeyUp);
    } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.which != SDL_TOUCH_MOUSEID) { // ignore synthetic mouse events
        result.mouseButtonDown = true;
        result.touchPoint = {e.button.x, e.button.y};
    } else if (e.type == SDL_FINGERDOWN) {
        result.touchPoint = {toInt(e.tfinger.x * rendererSize.x), toInt(e.tfinger.y * rendererSize.y)};
        if (e.tfinger.x < 0.5) {
            touchDownFn(leftTouch, result.leftTouchDown);
        } else {
            touchDownFn(rightTouch, result.rightTouchDown);
        }
    } else if (e.type == SDL_FINGERUP) {
        touchUpFn(leftTouch, result.leftTouchUp);
        touchUpFn(rightTouch, result.rightTouchUp);
    } else if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            result.focusLost = true;
        } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            result.focusGained = true;
        }
    }

    return result;
}
