#ifndef TRIPPINONTUBS_SCALE_H
#define TRIPPINONTUBS_SCALE_H

#include <string>
#include "engine/Point.h"

namespace trippin {
    class Scale {
    public:
        Scale(std::string name, int deviceFactor, int engineFactor);
        const std::string& getName() const;
        int getDeviceFactor() const;
        int getEngineFactor() const;
        int getDeviceEngineFactor() const;
    private:
        const std::string name;
        const int deviceFactor;
        const int engineFactor;
        const int deviceEngineFactor;
    };
}

#endif
