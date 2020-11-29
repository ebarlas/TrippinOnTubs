#ifndef TRIPPIN_POINT_H
#define TRIPPIN_POINT_H

#include <ostream>

namespace trippin {
    template<class T>
    struct Point {
        T x{};
        T y{};
        Point &operator+=(const Point<T> &right);
        Point operator+(const Point<T> &right) const;
        Point operator-(const Point<T> &right) const;
        T operator*(const Point<T> &right) const;
        Point operator*(T right) const;
        Point operator/(T right) const;
        T sumOfSquares() const;
        bool operator==(const Point<T> &p) const;
        operator bool() const;
        friend std::ostream &operator<<(std::ostream &out, const Point<T> &p);
    };
}

template<class T>
trippin::Point<T> &trippin::Point<T>::operator+=(const trippin::Point<T> &right) {
    x += right.x;
    y += right.y;
    return *this;
}

template<class T>
trippin::Point<T> trippin::Point<T>::operator+(const trippin::Point<T> &right) const {
    return {x + right.x, y + right.y};
}

template<class T>
trippin::Point<T> trippin::Point<T>::operator-(const trippin::Point<T> &right) const {
    return {x - right.x, y - right.y};
}

template<class T>
T trippin::Point<T>::operator*(const trippin::Point<T> &right) const {
    return x * right.x + y * right.y;
}

template<class T>
trippin::Point<T> trippin::Point<T>::operator*(T right) const {
    return {x * right, y * right};
}

template<class T>
T trippin::Point<T>::sumOfSquares() const {
    return x * x + y * y;
}

template<class T>
std::ostream &operator<<(std::ostream &out, const trippin::Point<T> &v) {
    out << v.x << " " << v.y;
    return out;
}

template<class T>
bool trippin::Point<T>::operator==(const trippin::Point<T> &p) const {
    return x == p.x && y == p.y;
}

template<class T>
trippin::Point<T> trippin::Point<T>::operator/(T right) const {
    return {x / right, y / right};
}

template<class T>
trippin::Point<T>::operator bool() const {
    return x || y;
}

#endif
