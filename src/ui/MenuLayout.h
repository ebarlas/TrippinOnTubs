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
        template<typename... Sprites>
        explicit
        MenuLayout(Point<int> windowSize, Uint32 duration, const RenderClock &renderClock, Sprites &&... sprites);
        void reset();
        void render();
        bool contains(int index, Point<int> point) const;
    private:
        struct Item {
            const Sprite *sprite;
            Point<int> position;
        };
        Point<int> windowSize;
        std::array<Item, N> items;
        Interpolator interpolator;
        void init();
        const Item &maxItem() const;
    };
}

template<std::size_t N>
template<typename... Sprites>
trippin::MenuLayout<N>::MenuLayout(
        Point<int> windowSize,
        Uint32 duration,
        const RenderClock &renderClock,
        Sprites &&... sprites):
        windowSize(windowSize),
        interpolator(renderClock, static_cast<int>(duration)) {
    static_assert(sizeof...(sprites) == N);
    int n = 0;
    auto fn = [this, &n](const Sprite &s) { items[n++].sprite = &s; };
    (fn(sprites), ...);
    init();
}

template<std::size_t N>
void trippin::MenuLayout<N>::init() {
    int menuHeight = 0;
    for (auto &item: items) {
        menuHeight += item.sprite->getDeviceSize().y;
    }

    int itemY = 0;
    for (auto &item: items) {
        item.position.x = (windowSize.x - item.sprite->getDeviceSize().x) / 2;
        item.position.y = (windowSize.y - menuHeight) / 2 + itemY;
        itemY += item.sprite->getDeviceSize().y;
    }

    auto &max = maxItem();
    interpolator.setMagnitude(max.position.x + max.sprite->getDeviceSize().x);
    interpolator.setOffset(-max.sprite->getDeviceSize().x);
}

template<std::size_t N>
void trippin::MenuLayout<N>::reset() {
    interpolator.reset();
}

template<std::size_t N>
void trippin::MenuLayout<N>::render() {
    auto &max = maxItem();
    int x = interpolator.interpolate();
    for (auto &item: items) {
        Point<int> p{x + item.position.x - max.position.x, item.position.y};
        item.sprite->renderDevice(p, 0);
    }
}

template<std::size_t N>
const typename trippin::MenuLayout<N>::Item &trippin::MenuLayout<N>::maxItem() const {
    auto cmp = [](const Item &a, const Item &b) {
        return a.sprite->getDeviceSize().x < b.sprite->getDeviceSize().x;
    };
    return *std::max_element(items.begin(), items.end(), cmp);
}

template<std::size_t N>
bool trippin::MenuLayout<N>::contains(int index, Point<int> point) const {
    auto &item = items[index];
    auto size = item.sprite->getDeviceSize();
    Rect<int> r{item.position.x, item.position.y, size.x, size.y};
    return r.contains(point);
}

#endif
