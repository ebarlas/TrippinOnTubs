#ifndef TRIPPIN_UNITS_H
#define TRIPPIN_UNITS_H

#include <cstdint>
#include "engine/Rect.h"

namespace trippin {
    class Units {
    public:
        Units(int engineFactor, int spriteFactor) :
                engineFactor(engineFactor),
                spriteFactor(spriteFactor) {}

        int getEngineFactor() const {
            return engineFactor;
        }

        int getSpriteFactor() const {
            return spriteFactor;
        }

        int baseToSprite(int right) const {
            return right * spriteFactor;
        }

        Point<int> baseToSprite(Point<int> right) const {
            return {baseToSprite(right.x), baseToSprite(right.y)};
        }

        Rect<int> baseToSprite(Rect<int> right) const {
            return {
                    baseToSprite(right.x),
                    baseToSprite(right.y),
                    baseToSprite(right.w),
                    baseToSprite(right.h)};
        }

        int baseToEngine(int base) const {
            return base * engineFactor;
        }

        int_fast64_t spriteToEngine(int right) const {
            return (right * engineFactor) / spriteFactor;
        }

        Point<int_fast64_t> spriteToEngine(const Point<int> &right) const {
            return {spriteToEngine(right.x), spriteToEngine(right.y)};
        }

        Rect<int_fast64_t> spriteToEngine(const Rect<int> &right) const {
            return {spriteToEngine(right.x),
                    spriteToEngine(right.y),
                    spriteToEngine(right.w),
                    spriteToEngine(right.h)};
        }

        int engineToSprite(int_fast64_t engine) const {
            return (engine * spriteFactor) / engineFactor;
        }

        Point<int> engineToSprite(const Point<int_fast64_t> &right) const {
            return {engineToSprite(right.x), engineToSprite(right.y)};
        }

    private:
        const int engineFactor;
        const int spriteFactor;
    };
}

#endif
