#include "Scale.h"

trippin::Scale::Scale(std::string name, int deviceFactor, int engineFactor) :
        name(std::move(name)),
        deviceFactor(deviceFactor),
        engineFactor(engineFactor),
        deviceEngineFactor(engineFactor / deviceFactor) {

}

const std::string &trippin::Scale::getName() const {
    return name;
}

int trippin::Scale::getDeviceFactor() const {
    return deviceFactor;
}

int trippin::Scale::getEngineFactor() const {
    return engineFactor;
}

int trippin::Scale::getDeviceEngineFactor() const {
    return deviceEngineFactor;
}
