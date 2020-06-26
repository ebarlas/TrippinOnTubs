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
        friend std::ostream &operator<<(std::ostream &out, const Vector<T> &v);
    };

    template<class T>
    Vector<T> &Vector<T>::operator+=(const Vector<T> &right) {
        x += right.x;
        y += right.y;
        return *this;
    }

    template<class T>
    Vector<T> Vector<T>::operator+(const Vector<T> &right) {
        return {x + right.x, y + right.y};
    }

    template<class T>
    Vector<T> Vector<T>::operator-(const Vector<T> &right) {
        return {x - right.x, y - right.y};
    }

    template<class T>
    T Vector<T>::operator*(const Vector<T> &right) const {
        return x * right.x + y * right.y;
    }

    template<class T>
    Vector<T> Vector<T>::operator*(T right) const {
        return {x * right, y * right};
    }

    template<class T>
    T Vector<T>::sumOfSquares() const {
        return x * x + y * y;
    }

    template<class T>
    std::ostream &operator<<(std::ostream &out, const Vector<T> &v) {
        out << v.x << " " << v.y;
        return out;
    }
}

#endif
