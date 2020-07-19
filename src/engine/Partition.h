#ifndef TRIPPIN_PARTITION_H
#define TRIPPIN_PARTITION_H

#include <vector>
#include "Object.h"

namespace trippin {
    struct Partition {
        std::vector<Object*> platforms;
        std::vector<Object*> objects;
        void add(Object *obj);
        void remove(const Object *obj);
    };
}

#endif
