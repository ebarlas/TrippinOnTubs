#ifndef TRIPPIN_RECT_H
#define TRIPPIN_RECT_H

#include <ostream>
#include "Point.h"
#include "Sides.h"

namespace trippin {
    template<class T>
    struct Rect {
        T x;
        T y;
        T w;
        T h;
        static bool valueInRange(T value, T min, T max);
        static trippin::Rect<T> intersect(const trippin::Rect<T> &a, const trippin::Rect<T> &b);
        Point<T> corner() const;
        T area() const;
        trippin::Rect<T> intersect(const trippin::Rect<T> &b) const;
        bool leftAlignedWith(const trippin::Rect<T> &other) const;
        bool rightAlignedWith(const trippin::Rect<T> &other) const;
        bool topAlignedWith(const trippin::Rect<T> &other) const;
        bool bottomAlignedWith(const trippin::Rect<T> &other) const;
        operator bool() const;
        static Sides collision(const trippin::Rect<T> &a, const trippin::Rect<T> &b);
        Sides collision(const trippin::Rect<T> &b) const;
        bool operator==(const Rect<T> &other) const;
        Rect<T> operator-(const Point<T> &other);
    };
}

template<class T>
bool trippin::Rect<T>::valueInRange(T value, T min, T max) {
    return (value >= min) && (value < max);
}

template<class T>
trippin::Rect<T> trippin::Rect<T>::intersect(const trippin::Rect<T> &a, const trippin::Rect<T> &b) {
    T x{}, y{}, w{}, h{};
    if (valueInRange(a.x, b.x, b.x + b.w)) {
        x = a.x;
        w = std::min(a.w, b.x + b.w - a.x);
    } else if (valueInRange(b.x, a.x, a.x + a.w)) {
        x = b.x;
        w = std::min(b.w, a.x + a.w - b.x);
    }
    if (valueInRange(a.y, b.y, b.y + b.h)) {
        y = a.y;
        h = std::min(a.h, b.y + b.h - a.y);
    } else if (valueInRange(b.y, a.y, a.y + a.h)) {
        y = b.y;
        h = std::min(b.h, a.y + a.h - b.y);
    }
    return {x, y, w, h};
}

template<class T>
trippin::Point<T> trippin::Rect<T>::corner() const {
    return {x, y};
}

template<class T>
trippin::Rect<T> trippin::Rect<T>::intersect(const trippin::Rect<T> &b) const {
    return intersect(*this, b);
}

template<class T>
bool trippin::Rect<T>::leftAlignedWith(const trippin::Rect<T> &other) const {
    return x == other.x;
}

template<class T>
bool trippin::Rect<T>::rightAlignedWith(const trippin::Rect<T> &other) const {
    return x + w == other.x + other.w;
}

template<class T>
bool trippin::Rect<T>::topAlignedWith(const trippin::Rect<T> &other) const {
    return y == other.y;
}

template<class T>
bool trippin::Rect<T>::bottomAlignedWith(const trippin::Rect<T> &other) const {
    return y + h == other.y + other.h;
}

template<class T>
trippin::Rect<T>::operator bool() const {
    return w > 0 && h > 0;
}

template<class T>
trippin::Sides trippin::Rect<T>::collision(const trippin::Rect<T> &a, const trippin::Rect<T> &b) {
    Sides sides{};
    if (a.y + a.h == b.y && (valueInRange(a.x, b.x, b.x + b.w) || valueInRange(b.x, a.x, a.x + a.w))) {
        sides.setBottom(true);
    } else if (a.y == b.y + b.h && (valueInRange(a.x, b.x, b.x + b.w) || valueInRange(b.x, a.x, a.x + a.w))) {
        sides.setTop(true);
    } else if (a.x + a.w == b.x && (valueInRange(a.y, b.y, b.y + b.h) || valueInRange(b.y, a.y, a.y + a.h))) {
        sides.setRight(true);
    } else if (a.x == b.x + b.w && (valueInRange(a.y, b.y, b.y + b.h) || valueInRange(b.y, a.y, a.y + a.h))) {
        sides.setLeft(true);
    }
    return sides;
}

template<class T>
trippin::Sides trippin::Rect<T>::collision(const trippin::Rect<T> &b) const {
    return collision(*this, b);
}

template<class T>
bool trippin::Rect<T>::operator==(const trippin::Rect<T> &other) const {
    return x == other.x && y == other.y && w == other.w && h == other.h;
}

template<class T>
T trippin::Rect<T>::area() const {
    return w * h;
}

template<class T>
trippin::Rect<T> trippin::Rect<T>::operator-(const trippin::Point<T> &other) {
    return {x - other.x, y - other.y, w, h};
}

#endif