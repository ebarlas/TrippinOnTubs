#include "NotificationManager.h"
#include "ui/DigitLayout.h"

trippin::NotificationManager::NotificationManager(
        const trippin::Sprite &text,
        const trippin::Sprite &digits,
        double msPerTick,
        int windowWidth,
        int topMargin,
        int rightMargin,
        trippin::SceneBuilder &sceneBuilder) :
        text(text),
        digits(digits),
        msPerTick(msPerTick),
        windowWidth(windowWidth),
        topMargin(topMargin),
        rightMargin(rightMargin),
        sceneBuilder(sceneBuilder) {

}

void trippin::NotificationManager::add(int value) {
    notifications[nextDisplayPos] = {value, 0};
    nextDisplayPos = (nextDisplayPos + 1) % notifications.size();
}

void trippin::NotificationManager::afterTick(int engineTicks) {
    for (auto &notification: notifications) {
        if (notification.value) {
            if (!notification.startTicks) {
                notification.startTicks = engineTicks;
            }
            render(engineTicks, notification);
        }
    }
}

void trippin::NotificationManager::render(int engineTicks, Notification &notification) {
    auto ticksElapsed = engineTicks - notification.startTicks;
    auto slideDuration = DURATION_SLIDE / msPerTick;
    auto pauseDuration = DURATION_PAUSE / msPerTick;
    auto pauseTicks = slideDuration + pauseDuration;

    auto digitsWidth = DigitLayout::measureWidth(digits, notification.value);
    auto totalWidth = text.getDeviceSize().x + digitsWidth;
    auto totalDistance = totalWidth + rightMargin;

    int distance;
    if (ticksElapsed < slideDuration)
        distance = static_cast<int>(totalDistance * decelInterpolation(ticksElapsed / slideDuration));
    else if (ticksElapsed < pauseTicks)
        distance = totalDistance;
    else
        distance = static_cast<int>(totalDistance * decelInterpolation((ticksElapsed - pauseDuration) / slideDuration));

    if (distance < 0) {
        notification.value = 0; // denotes path fully traveled, all done
        return;
    }

    auto x = windowWidth - distance;
    auto value = notification.value;
    sceneBuilder.dispatch([this, x, totalWidth, value]() {
        text.renderDevice({x, topMargin}, 0);
        DigitLayout::renderDigits(digits, {x + totalWidth, topMargin}, value);
    });
}

double trippin::NotificationManager::decelInterpolation(double input) {
    return (1.0 - (1.0 - input) * (1.0 - input));
}


