#ifndef TRIPPINONTUBS_SCALE_H
#define TRIPPINONTUBS_SCALE_H

#include <string>
#include "engine/Point.h"

namespace trippin {
    class Scale {
    public:
        Scale(std::string name, int multiplier);
        const std::string& getName() const;
        int getMultiplier() const;
    private:
        const std::string name;
        const int multiplier;
    };
}

#endif
