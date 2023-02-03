#include "NotificationDrawer.h"

trippin::NotificationDrawer::NotificationDrawer(
        double msPerTick,
        int windowWidth,
        int topMargin,
        int rightMargin,
        int padding,
        trippin::SceneBuilder &sceneBuilder) :
        msPerTick(msPerTick),
        windowWidth(windowWidth),
        topMargin(topMargin),
        rightMargin(rightMargin),
        padding(padding),
        sceneBuilder(sceneBuilder) {

}

void trippin::NotificationDrawer::add(std::function<void(Point<int>)> renderFn, trippin::Point<int> size) {
    entries[nextEntry] = {std::move(renderFn), size, 0};
    nextEntry = (nextEntry + 1) % entries.size();
}

void trippin::NotificationDrawer::afterTick(int engineTicks) {
    int top = topMargin; // accumulates height for each entry
    for (auto i = firstEntry; i != nextEntry; i = (i + 1) % entries.size()) { // walk active entries
        auto &entry = entries[i];
        if (entry.startTicks == 0) { // start the clock
            entry.startTicks = engineTicks;
        }
        auto height = entry.size.y + padding;
        if (entry.endTicks == 0) {
            render(engineTicks, entry, top);
            top += height;
        } else { // row collapses for a period after it is out of view
            auto collapseTicksElapsed = engineTicks - entry.endTicks;
            auto collapseTicksLimit = DURATION_COLLAPSE / msPerTick;
            if (collapseTicksElapsed > collapseTicksLimit) { // really over now
                entry.startTicks = entry.endTicks = 0;
                firstEntry = (firstEntry + 1) % entries.size();
            } else {
                top += static_cast<int>(height * (1.0 - (collapseTicksElapsed / collapseTicksLimit)));
            }
        }

    }
}

void trippin::NotificationDrawer::render(int engineTicks, Entry &entry, int top) {
    auto ticksElapsed = engineTicks - entry.startTicks;
    auto slideDuration = DURATION_SLIDE / msPerTick;
    auto pauseDuration = DURATION_PAUSE / msPerTick;
    auto pauseTicks = slideDuration + pauseDuration;

    auto totalDistance = entry.size.x + rightMargin;

    int distance;
    if (ticksElapsed < slideDuration)
        distance = static_cast<int>(totalDistance * decelInterpolation(ticksElapsed / slideDuration));
    else if (ticksElapsed < pauseTicks)
        distance = totalDistance;
    else
        distance = static_cast<int>(totalDistance * decelInterpolation((ticksElapsed - pauseDuration) / slideDuration));

    if (distance < 0) { // denotes path fully traveled, all done
        entry.endTicks = engineTicks;
        return;
    }

    auto &renderFn = entry.renderFn;
    auto left = windowWidth - distance;
    sceneBuilder.dispatch([left, top, renderFn]() {
        renderFn({left, top});
    });
}

double trippin::NotificationDrawer::decelInterpolation(double input) {
    return (1.0 - (1.0 - input) * (1.0 - input));
}
