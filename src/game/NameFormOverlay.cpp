#include "NameFormOverlay.h"

void trippin::NameFormOverlay::init(const Point<int> &ws, SpriteManager &spriteManager) {
    sprite = &spriteManager.get("alpha");
    windowSize = ws;
    cursor = 0;
}

void trippin::NameFormOverlay::render() {
    Point<int> size = sprite->getSize();
    int width = windowSize.x - (size.x * columns);
    int height = windowSize.y - (size.y * (rows + 1));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int digit = r * columns + c;
            if (digit < chars) {
                Point<int> pos{width / 2 + c * size.x, height / 2 + r * size.y};
                sprite->render(pos, digit);
            }
        }
    }

    int margin = size.x / (nameLength - 1);
    width = windowSize.x - (size.x * (nameLength + 1));
    for (int c = 0; c < nameLength; c++) {
        Point<int> pos{width / 2 + c * size.x + c * margin, height / 2 + rows * size.y};
        if (c < cursor) {
            sprite->render(pos, 37);
            sprite->render(pos, digits[c]);
        } else if (c == cursor) {
            sprite->render(pos, 36);
        } else {
            sprite->render(pos, 37);
        }
    }
}

void trippin::NameFormOverlay::onClick(const Point<int> &coords) {
    Point<int> size = sprite->getSize();
    int width = windowSize.x - (size.x * columns);
    int height = windowSize.y - (size.y * (rows + 1));

    Point<int> corner{width / 2, height / 2};
    Point<int> offset = coords - corner;
    int row = offset.y / size.y;
    int col = offset.x / size.x;
    if (row >= 0 && row < rows && col >= 0 && col < columns) {
        int digit = row * columns + col;
        digits[cursor] = digit;
        cursor++;
    }
}

bool trippin::NameFormOverlay::nameEntered() const {
    return cursor == nameLength;
}

std::string trippin::NameFormOverlay::name() const {
    std::string name;
    for (int digit : digits) {
        name += alphabet[digit];
    }
    return name;
}
