#ifndef TRIPPIN_NAME_H
#define TRIPPIN_NAME_H

#include <string>

namespace trippin {
    class Name {
    private:
        int name[5];
    public:
        int &operator[](int n);
        std::string encode();
        void decode(const std::string &str);
    };
}

#endif
