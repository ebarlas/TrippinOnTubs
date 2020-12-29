#ifndef TRIPPIN_EXCHANGE_H
#define TRIPPIN_EXCHANGE_H

#include "Guarded.h"

namespace trippin {
    // Exchange uses RAII to get and set on a wrapped Guarded reference.
    // The get function can be used to obtain a reference to the copied value for modification.
    template<class T>
    class Exchange {
    private:
        T value;
        Guarded <T> &guarded;
    public:
        Exchange(Guarded <T> &guarded);
        ~Exchange();
        T &get();
    };
}

template<class T>
trippin::Exchange<T>::Exchange(Guarded <T> &guarded) : guarded(guarded) {
    value = guarded.get();
}

template<class T>
trippin::Exchange<T>::~Exchange() {
    guarded.set(value);
}

template<class T>
T &trippin::Exchange<T>::get() {
    return value;
}

#endif