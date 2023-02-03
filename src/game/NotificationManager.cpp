#include "NotificationManager.h"
#include "ui/DigitLayout.h"

trippin::NotificationManager::NotificationManager(
        const trippin::Sprite &text,
        const trippin::Sprite &digits,
        NotificationDrawer &notificationDrawer) :
        text(text),
        digits(digits),
        notificationDrawer(notificationDrawer) {

}

void trippin::NotificationManager::add(int value) {
    auto digitsWidth = DigitLayout::measureWidth(digits, value);
    auto totalWidth = text.getDeviceSize().x + digitsWidth;

    auto renderFn = [this, value, totalWidth](Point<int> p) {
        text.renderDevice(p, 0);
        DigitLayout::renderDigits(digits, {p.x + totalWidth, p.y}, value);
    };

    notificationDrawer.add(renderFn, {totalWidth, text.getDeviceSize().y});
}