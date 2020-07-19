#ifndef TRIPPIN_SNAPQUEUE_H
#define TRIPPIN_SNAPQUEUE_H

#include <queue>
#include <utility>
#include "engine/Object.h"

namespace trippin {
    class SnapQueue {
    private:
        struct Compare {
            bool operator()(const std::pair<Object *, Sides> &left, const std::pair<Object *, Sides> &right);
        };

        using Pair = std::pair<Object *, Sides>;
        std::priority_queue<Pair, std::vector<Pair>, Compare> q;
        void removeVisited();
    public:
        SnapQueue();
        void push(Object *obj);
        void push(Object *obj, Sides sides);
        Object *pop();
        bool empty();
    };
}

#endif