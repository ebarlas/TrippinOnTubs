#include <iostream>
#include <chrono>

using namespace std;

enum class UnitType {
    base,
    engine,
    device
};

template <UnitType From, UnitType To>
struct Factor {
    static int numerator;
    static int denominator;
};

template <>
int Factor<UnitType::base, UnitType::engine>::numerator = 1;

template <>
int Factor<UnitType::base, UnitType::engine>::denominator = 1;

template <>
int Factor<UnitType::engine, UnitType::device>::numerator = 1;

template <>
int Factor<UnitType::engine, UnitType::device>::denominator = 1;

template<typename T, UnitType V>
struct Unit {
    T value;

    T *operator->() {
        return &value;
    }

    const T *operator->() const {
        return &value;
    }

    T &operator*() {
        return value;
    }

    operator T() {
        return value;
    }

    const T &operator*() const {
        return value;
    }

    Unit() = default;

    Unit(T val) {
        value = val;
    }

    template<UnitType Z>
    Unit(const Unit<T, Z> &right) {
        value = (right.value * Factor<Z, V>::numerator) / Factor<Z, V>::denominator;
    }

    template<UnitType Z>
    Unit<T, V> &operator=(const Unit<T, Z> &right) {
        value = (right.value * Factor<Z, V>::numerator) / Factor<Z, V>::denominator;
        return *this;
    }
};

//int main() {
//    // these are strictly runtime configurations!
//    engineFactor = 1'000; // engine units are 1,000x base units
//    deviceFactor = 5; // device units are 5x base units
//
//    Unit<int, UnitType::base> baseUnits{42};
//    Unit<int, UnitType::engine> engineUnits = baseUnits;
//    Unit<int, UnitType::device> deviceUnits = engineUnits;
//    return deviceUnits;
//}

int main() {
    std::chrono::duration<int, std::milli> millis{42};
    std::chrono::duration<int, std::micro> micros = millis;
    cout << micros.count() << endl;

    int engineFactor;
    cout << "Engine factor: ";
    cin >> engineFactor;

    int deviceFactor;
    cout << "Device factor: ";
    cin >> deviceFactor;

    Factor<UnitType::base, UnitType::engine>::numerator = engineFactor;
    Factor<UnitType::base, UnitType::engine>::denominator = 1;

    Factor<UnitType::engine, UnitType::device>::numerator = deviceFactor;
    Factor<UnitType::engine, UnitType::device>::denominator = engineFactor;

    Unit<int, UnitType::base> baseUnits{};
    cout << "Base units: ";
    cin >> baseUnits.value;

    Unit<int, UnitType::engine> engineUnits = baseUnits;
    cout << "Engine units: " << engineUnits.value << endl;

    Unit<int, UnitType::device> deviceUnits = engineUnits;
    cout << "Sprite units: " << deviceUnits.value << endl;
}