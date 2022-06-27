#include "ComboManager.h"
#include "ui/DigitLayout.h"
#include "engine/Convert.h"

trippin::ComboManager::ComboManager(
        SpriteManager &spriteManager,
        ScoreTicker &scoreTicker,
        double msPerTick,
        Point<int> windowSize,
        int margin,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        scoreTicker(scoreTicker),
        comboText(spriteManager.get("combo")),
        digits(spriteManager.get("digits")),
        msPerTick(msPerTick),
        windowSize(windowSize),
        margin(margin),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex) {

}

void trippin::ComboManager::reset() {
    if (hits >= 3) {
        scoreTicker.add(10 * hits);
        displays[nextDisplayPos] = {hits, 0};
        nextDisplayPos = (nextDisplayPos + 1) % 3;
    }
    hits = 0;
}

void trippin::ComboManager::recordHit() {
    hits++;
}

void trippin::ComboManager::afterTick(Uint32 engineTicks) {
    for (auto &display: displays) {
        if (display.points) {
            if (!display.startTicks) {
                display.startTicks = engineTicks;
            }

            render(engineTicks, display);
        }
    }
}

void trippin::ComboManager::render(Uint32 engineTicks, Display &display) {
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
    }, zIndex);
}

double trippin::ComboManager::decelInterpolation(float input) {
    return (float) (1.0f - (1.0f - input) * (1.0f - input));
}