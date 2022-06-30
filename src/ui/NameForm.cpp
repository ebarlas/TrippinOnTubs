#include "NameForm.h"

trippin::NameForm::NameForm(const Point<int> &windowSize, SpriteManager &spriteManager) :
        windowSize(windowSize),
        sprite(spriteManager.get("alpha")),
        cursor(0),
        entered(false),
        gridSize(computeGridSize(windowSize, sprite.getDeviceSize())),
        padding(sprite.getDeviceSize().x / paddingDivisor) {
    name.resize(3);
}

trippin::Point<int> trippin::NameForm::computeGridSize(trippin::Point<int> windowSize, trippin::Point<int> spriteSize) {
    int padding = spriteSize.x / paddingDivisor;
    return Point<int>{
            windowSize.x - ((spriteSize.x + padding * 2) * columns),
            windowSize.y - ((spriteSize.y + padding * 2) * (rows + 1))};
}

void trippin::NameForm::reset() {
    cursor = 0;
}

void trippin::NameForm::render() {
    auto size = sprite.getDeviceSize();
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int position = r * columns + c;
            Point<int> pos{gridSize.x / 2 + c * (size.x + padding * 2) + padding,
                           gridSize.y / 2 + r * (size.y + padding * 2) + padding};
            // 26 letters + 1 delete + 1 enter
            if (position < 28) {
                sprite.renderDevice(pos, position + 10);
            }
        }
    }

    int margin = size.x / (nameLength - 1);
    auto width = windowSize.x - (size.x * (nameLength + 1));
    for (int c = 0; c < nameLength; c++) {
        Point<int> pos{width / 2 + c * size.x + c * margin,
                       gridSize.y / 2 + rows * (size.y + padding * 2) + padding};
        if (c < cursor) {
            sprite.renderDevice(pos, 39);
            sprite.renderDevice(pos, name[c] - 'A' + 10);
        } else if (c == cursor) {
            sprite.renderDevice(pos, 38);
        } else {
            sprite.renderDevice(pos, 39);
        }
    }
}

void trippin::NameForm::onClick(const Point<int> &coords) {
    auto size = sprite.getDeviceSize();
    Point<int> corner{gridSize.x / 2, gridSize.y / 2};
    Point<int> offset = coords - corner;
    int row = offset.y / (size.y + padding * 2);
    int col = offset.x / (size.x + padding * 2);
    if (row >= 0 && row < rows && col >= 0 && col < columns) { // within grid?
        int position = row * columns + col;
        if (position < 26 && cursor < nameLength) { // letter click
            name[cursor] = static_cast<char>(position + 'A');
            cursor++;
        } else if (position == 26 && cursor > 0) { // delete click
            cursor--;
        } else if (position == 27 && cursor == nameLength) { // enter click
            entered = true;
        }
    }
}

bool trippin::NameForm::nameEntered() const {
    return entered;
}

const std::string &trippin::NameForm::getName() const {
    return name;
}
