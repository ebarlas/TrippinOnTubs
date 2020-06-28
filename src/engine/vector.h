#ifndef TRIPPIN_VECTOR_H
#define TRIPPIN_VECTOR_H

#include <ostream>

namespace trippin {
    template<class T>
    struct Vector {
        T x;
        T y;
        Vector &operator+=(const Vector<T> &right);
        Vector operator+(const Vector<T> &right);
        Vector operator-(const Vector<T> &right);
        T operator*(const Vector<T> &right) const;
        Vector operator*(T right) const;
        T sumOfSquares() const;
        bool operator==(const Vector<T> &v) const;
        friend std::ostream &operator<<(std::ostream &out, const Vector<T> &v);
    };

    using DoubleVector = Vector<double>;
    using IntVector = Vector<double>;
}

template<class T>
trippin::Vector<T> &trippin::Vector<T>::operator+=(const trippin::Vector<T> &right) {
    x += right.x;
    y += right.y;
    return *this;
}

template<class T>
trippin::Vector<T> trippin::Vector<T>::operator+(const trippin::Vector<T> &right) {
    return {x + right.x, y + right.y};
}

template<class T>
trippin::Vector<T> trippin::Vector<T>::operator-(const trippin::Vector<T> &right) {
    return {x - right.x, y - right.y};
}

template<class T>
T trippin::Vector<T>::operator*(const trippin::Vector<T> &right) const {
    return x * right.x + y * right.y;
}

template<class T>
trippin::Vector<T> trippin::Vector<T>::operator*(T right) const {
    return {x * right, y * right};
}

template<class T>
T trippin::Vector<T>::sumOfSquares() const {
    return x * x + y * y;
}

template<class T>
std::ostream &operator<<(std::ostream &out, const trippin::Vector<T> &v) {
    out << v.x << " " << v.y;
    return out;
}

template<class T>
bool trippin::Vector<T>::operator==(const trippin::Vector<T> &v) const {
    return x == v.x && y == v.y;
}

#endif
