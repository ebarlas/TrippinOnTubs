#include "Marquee.h"

trippin::Marquee::Marquee(
        double scrollPxPerMs,
        const Point<int> &windowSize,
        const Sprite &characters,
        const RenderClock &renderClock,
        int margin) :
        scrollPxPerMs(scrollPxPerMs),
        charWidths(makeCharWidths()),
        windowSize(windowSize),
        characters(characters),
        renderClock(renderClock),
        margin(margin) {

}

void trippin::Marquee::start(const std::string &m) {
    startTicks = renderClock.getTicks();
    message = compile(m);
}

std::vector<trippin::Marquee::Char> trippin::Marquee::compile(const std::string &msg) const {
    auto deviceSize = characters.getDeviceSize();
    auto origWidth = deviceSize.x;
    auto y = 0;
    auto x = 0;
    auto lineWidth = -1;
    std::vector<trippin::Marquee::Char> chars;
    for (auto it = msg.begin(); it != msg.end(); it++) {
        auto c = *it;
        auto frame = getFrame(c);
        auto adjustedWidth = static_cast<int>(static_cast<float>(origWidth) * charWidths.at(c));
        auto diff = origWidth - adjustedWidth;
        auto diffHalf = diff / 2;
        if (frame >= 0) { // no frame for space and newline chars
            if (lineWidth < 0) {
                lineWidth = calculateLineWidth(msg, it);
            }
            chars.push_back({frame, x + windowSize.x / 2 - lineWidth / 2 - diffHalf, y + windowSize.y});
        }
        x += adjustedWidth;
        if (endOfLine(c, x)) {
            x = 0;
            y += deviceSize.y + margin;
            lineWidth = -1; // signal that line width need re-calc
        }
    }
    return chars;
}

bool trippin::Marquee::started() const {
    return !message.empty();
}

void trippin::Marquee::render() {
    auto charHeight = characters.getDeviceSize().y;
    auto elapsedMs = static_cast<double>((renderClock.getTicks() - startTicks).count());
    auto scrollPx = static_cast<int>(scrollPxPerMs * elapsedMs);
    for (auto &c : message) {
        Point<int> p{c.x, c.y + scrollPx};
        if (p.y + charHeight >= 0 && p.y <= windowSize.y) {
            characters.renderDevice(p, c.frame);
        }
    }
}

int trippin::Marquee::calculateLineWidth(const std::string &s, std::string::const_iterator it) const {
    int origWidth = characters.getDeviceSize().x;
    int lineWidth = 0;
    while (it != s.end()) {
        char c = *it;
        int newWidth = static_cast<int>(static_cast<float>(origWidth) * charWidths.at(c));
        lineWidth += newWidth;
        if (endOfLine(c, lineWidth)) {
            lineWidth -= newWidth; // remove SP or NL contribution
            break;
        }
        it++;
    }
    return lineWidth;
}

bool trippin::Marquee::endOfLine(char c, int lineWidth) const {
    return c == '\n' || (c == ' ' && lineWidth > windowSize.x / 4);
}

int trippin::Marquee::getFrame(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    if (c == '.') {
        return 40;
    }
    if (c == ',') {
        return 41;
    }
    if (c == '\'') {
        return 42;
    }
    if (c == '!') {
        return 43;
    }
    return -1; // no frame for SP or NL
}

std::unordered_map<char, float> trippin::Marquee::makeCharWidths() {
    std::unordered_map<char, float> m;
    for (auto &[c, f]: CHAR_WIDTHS) {
        m[c] = f;
    }
    return m;
}