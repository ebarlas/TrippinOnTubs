#ifndef TRIPPIN_GUARDED_H
#define TRIPPIN_GUARDED_H

#include "Lock.h"

namespace trippin {
    // Synchronization utility that guards access to a wrapped value with a mutex.
    // set() copies to the wrapped value guarded by mutex
    // get() makes a copy of the wrapped value guarded by mutex
    // ref() returns a reference to the wrapped value unguarded
    template<class T>
    class Guarded {
    private:
        T value;
        Mutex mutex;
    public:
        void set(const T &value);
        T get() const;
        T& ref();
    };
}

template<class T>
void trippin::Guarded<T>::set(const T &val) {
    Lock lock(mutex);
    value = val;
}

template<class T>
T trippin::Guarded<T>::get() const {
    Lock lock(mutex);
    return value;
}

template<class T>
T &trippin::Guarded<T>::ref() {
    return value;
}

#endif
