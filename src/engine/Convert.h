#ifndef TRIPPIN_CONVERT_H
#define TRIPPIN_CONVERT_H

#include <cmath>
#include "Point.h"

namespace trippin {
    inline int toInt(double d) {
        return static_cast<int>(std::round(d));
    }

    inline double toDouble(int n) {
        return static_cast<double>(n);
    }

    inline Point<int> toInt(Point<double> p) {
        return {toInt(p.x), toInt(p.y)};
    }
}

#endif