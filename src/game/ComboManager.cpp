#include "ComboManager.h"
#include "ui/DigitLayout.h"
#include "engine/Convert.h"

trippin::ComboManager::ComboManager(
        SpriteManager &spriteManager,
        int minComboHits,
        double msPerTick,
        Point<int> windowSize,
        int margin,
        SceneBuilder &sceneBuilder) :
        minComboHits(minComboHits),
        msPerTick(msPerTick),
        windowSize(windowSize),
        margin(margin),
        comboText(spriteManager.get("combo")),
        digits(spriteManager.get("digits")),
        sceneBuilder(sceneBuilder) {

}

int trippin::ComboManager::reset() {
    int result = 0;
    if (hits >= minComboHits) {
        result = hits;
        displays[nextDisplayPos] = {hits, 0};
        nextDisplayPos = (nextDisplayPos + 1) % 3;
    }
    hits = 0;
    return result;
}

void trippin::ComboManager::recordHit() {
    hits++;
}

void trippin::ComboManager::afterTick(int engineTicks) {
    for (auto &display: displays) {
        if (display.points) {
            if (!display.startTicks) {
                display.startTicks = engineTicks;
            }

            render(engineTicks, display);
        }
    }
}

void trippin::ComboManager::render(int engineTicks, Display &display) {
    auto ticksElapsed = engineTicks - display.startTicks;
    auto slideDuration = DURATION_SLIDE / msPerTick;
    auto pauseDuration = DURATION_PAUSE / msPerTick;
    auto pauseTicks = slideDuration + pauseDuration;

    auto digitsWidth = DigitLayout::measureWidth(digits, display.points);
    auto totalWidth = comboText.getDeviceSize().x + digitsWidth;
    auto totalDistance = totalWidth + margin;

    auto distance = [&]() {
        if (ticksElapsed < slideDuration)
            return toInt(totalDistance * decelInterpolation(ticksElapsed / slideDuration));
        if (ticksElapsed < pauseTicks)
            return totalDistance;
        return toInt(totalDistance * decelInterpolation((ticksElapsed - pauseDuration) / slideDuration));
    }();

    if (distance < 0) {
        display.points = 0;
        return;
    }

    auto x = windowSize.x - distance;
    auto y = windowSize.y / 4;
    auto points = display.points;
    sceneBuilder.dispatch([this, x, y, totalWidth, points]() {
        comboText.renderDevice({x, y}, 0);
        DigitLayout::renderDigits(digits, {x + totalWidth, y}, points);
    });
}

double trippin::ComboManager::decelInterpolation(double input) {
    return (1.0 - (1.0 - input) * (1.0 - input));
}