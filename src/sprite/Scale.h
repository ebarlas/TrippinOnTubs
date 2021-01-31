#ifndef TRIPPINONTUBS_SCALE_H
#define TRIPPINONTUBS_SCALE_H

#include <string>
#include "engine/Point.h"

namespace trippin {
    class Scale {
    public:
        Scale(std::string name, double multiplier);
        const std::string& getName() const;
        double getMultiplier() const;
    private:
        const std::string name;
        const double multiplier;
    };
}

#endif
