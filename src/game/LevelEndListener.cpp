#include "LevelEndListener.h"

trippin::LevelEndListener::LevelEndListener(const trippin::Goggin &goggin, ScoreTicker &scoreTicker)
        : goggin(goggin), scoreTicker(scoreTicker) {}

void trippin::LevelEndListener::afterTick(int engineTicks) {
    if (goggin.belowUniverse() || goggin.rightOfUniverse()) {
        ended = true;
        scoreTicker.close();
    }
}

bool trippin::LevelEndListener::hasEnded() const {
    return ended;
}