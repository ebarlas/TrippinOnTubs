#ifndef TRIPPIN_FRACTION_H
#define TRIPPIN_FRACTION_H

namespace trippin {
    template<class T>
    struct Fraction {
        T numerator{0};
        T denominator{1};

        explicit operator T() const {
            return numerator / denominator;
        }

        template<class V>
        explicit operator Fraction<V>() const;

        bool isOne() const {
            return numerator == denominator;
        }

        Fraction<T> flip() const {
            return {denominator, numerator};
        }

        Fraction<T> operator*(const T &right) const {
            return {numerator * right, denominator};
        }

        Fraction<T> operator*(const Fraction<T> &right) const {
            return {numerator * right.numerator, denominator * right.denominator};
        }

        Fraction<T> &operator*=(const T &right) const {
            numerator *= right;
            return *this;
        }

        Fraction<T> &operator*=(const Fraction<T> &right) const {
            numerator *= right.numerator;
            denominator *= right.denominator;
            return *this;
        }
    };

    template<>
    template<>
    inline Fraction<int>::operator Fraction<int_fast64_t>() const {
        return {numerator, denominator};
    }
}

#endif