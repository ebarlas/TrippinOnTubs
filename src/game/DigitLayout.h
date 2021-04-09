#ifndef TRIPPIN_DIGITLAYOUT_H
#define TRIPPIN_DIGITLAYOUT_H

#include "sprite/Sprite.h"

namespace trippin {
    class DigitLayout {
    public:
        inline static void renderDigits(
                const Sprite &digits,
                const Point<int> &right,
                int value,
                const Camera *camera = nullptr) {
            int x = right.x;
            do {
                auto digit = value % 10;
                value /= 10;
                if (camera)
                    digits.render({x, right.y}, digit, *camera);
                else
                    digits.render({x, right.y}, digit);
                x -= digits.getSize().x;
            } while (value > 0);
        }

        inline static int countDigits(const Sprite &digits, int value) {
            int numDigits = 0;
            do {
                value /= 10;
                numDigits++;
            } while (value > 0);
            return numDigits;
        }
    };
}

#endif
