#ifndef TRIPPIN_OPTIONAL_H
#define TRIPPIN_OPTIONAL_H

namespace trippin {
    template <class T>
    class Optional {
    private:
        T value{};
        bool present{};
    public:
        bool isPresent() const;
        const T &get() const;
        void set(const T& val);
    };
}

template<class T>
bool trippin::Optional<T>::isPresent() const {
    return present;
}

template<class T>
const T &trippin::Optional<T>::get() const {
    return value;
}

template<class T>
void trippin::Optional<T>::set(const T &val) {
    present = true;
    value = val;
}

#endif
