#ifndef TRIPPIN_RANDOM_H
#define TRIPPIN_RANDOM_H

#include <random>
#include <array>

namespace trippin {
    template<class T = int, T _min = 0, T _max = 1'000'000>
    class Random {
    public:
        Random() : gen(rd()), distrib(_min, _max) {}

        T next() {
            return distrib(gen);
        }

        T min() {
            return _min;
        }

        T max() {
            return _max;
        }
    private:
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_int_distribution<T> distrib;
    };
}

#endif
