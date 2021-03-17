#ifndef TRIPPIN_MENULAYOUT_H
#define TRIPPIN_MENULAYOUT_H

#include <array>
#include <algorithm>
#include <numeric>
#include "sprite/Sprite.h"
#include "Interpolator.h"

namespace trippin {
    template<std::size_t N>
    class MenuLayout {
    public:
        void setSprite(int index, const Sprite *sprite);
        void init(const Point<int> &windowSize, Uint32 duration);
        void reset();
        void render();
        bool contains(int index, Point<int> point) const;
    private:
        struct Item {
            const Sprite *sprite;
            Point<int> position;
        };
        std::array<Item, N> items;
        Interpolator interpolator;
        const Item& maxItem() const;
    };
}

template<std::size_t N>
void trippin::MenuLayout<N>::setSprite(int index, const Sprite *sprite) {
    items[index].sprite = sprite;
}

template<std::size_t N>
void trippin::MenuLayout<N>::init(const Point<int> &windowSize, Uint32 duration) {
    int menuHeight = 0;
    for (auto &item : items) {
        menuHeight += item.sprite->getSize().y;
    }

    int itemY = 0;
    for (auto &item : items) {
        item.position.x = (windowSize.x - item.sprite->getSize().x) / 2;
        item.position.y = (windowSize.y - menuHeight) / 2 + itemY;
        itemY += item.sprite->getSize().y;
    }

    auto &max = maxItem();
    interpolator.init(duration, max.position.x + max.sprite->getSize().x);
}

template<std::size_t N>
void trippin::MenuLayout<N>::reset() {
    interpolator.reset();
}

template<std::size_t N>
void trippin::MenuLayout<N>::render() {
    auto &max = maxItem();
    int x = interpolator.interpolate() - max.sprite->getSize().x;
    for (auto &item : items) {
        Point<int> p{x + item.position.x - max.position.x, item.position.y};
        item.sprite->render(p, 0);
    }
}

template<std::size_t N>
const typename trippin::MenuLayout<N>::Item &trippin::MenuLayout<N>::maxItem() const {
    auto cmp = [](const Item &a, const Item &b) {
        return a.sprite->getSize().x < b.sprite->getSize().x;
    };
    return *std::max_element(items.begin(), items.end(), cmp);
}

template<std::size_t N>
bool trippin::MenuLayout<N>::contains(int index, Point<int> point) const {
    auto &item = items[index];
    auto size = item.sprite->getSize();
    Rect<int> r{item.position.x, item.position.y, size.x, size.y};
    return r.contains(point);
}

#endif
