#ifndef TRIPPIN_EXCHANGE_H
#define TRIPPIN_EXCHANGE_H

#include "Guarded.h"

namespace trippin {
    template<class T>
    class AutoGuard {
    private:
        T& ref;
        Guarded <T> &guarded;
    public:
        AutoGuard(T& ref, Guarded <T> &guarded);
        ~AutoGuard();
    };
}

template<class T>
trippin::AutoGuard<T>::AutoGuard(T& ref, Guarded <T> &guarded) : ref(ref), guarded(guarded) {

}

template<class T>
trippin::AutoGuard<T>::~AutoGuard() {
    guarded.set(ref);
}

#endif