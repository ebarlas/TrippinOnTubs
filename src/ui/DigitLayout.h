#ifndef TRIPPIN_DIGITLAYOUT_H
#define TRIPPIN_DIGITLAYOUT_H

#include <functional>
#include "sprite/Sprite.h"

namespace trippin {
    class DigitLayout {
    public:
        inline static void renderDigits(
                const Sprite &digits,
                const Point<int> &right, // engine scale
                int value,
                const Camera *camera) {
            auto x = right.x;
            auto y = right.y;
            forEachDigit(value, [&x, &y, &digits, camera](int digit) {
                x -= digits.getEngineSize().x;
                digits.renderEngine({x, y}, digit, *camera);
            });
        }

        inline static void renderDigits(
                const Sprite &digits,
                const Point<int> &right, // device scale
                int value) {
            auto x = right.x;
            auto y = right.y;
            forEachDigit(value, [&x, &y, &digits](int digit) {
                x -= digits.getDeviceSize().x;
                digits.renderDevice({x, y}, digit);
            });
        }

        inline static void forEachDigit(int value, const std::function<void(int)> &fn) {
            do {
                auto digit = value % 10;
                fn(digit);
                value /= 10;
            } while (value > 0);
        }

        inline static int countDigits(int value) {
            int numDigits = 0;
            forEachDigit(value, [&numDigits](int) { numDigits++; });
            return numDigits;
        }

        inline static int measureWidth(const Sprite &digits, int value) {
            return countDigits(value) * digits.getDeviceSize().x;
        }
    };
}

#endif
