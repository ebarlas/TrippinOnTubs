#ifndef TRIPPIN_COLLISIONTABLE_H
#define TRIPPIN_COLLISIONTABLE_H

#include <unordered_set>
#include "engine/Object.h"
#include "engine/Point.h"

namespace trippin {
    // Data structure to aid in the detection of redundant collisions in consecutive engine ticks.
    class CollisionTable {
    private:
        struct Key {
            int id1;
            int id2;
            Point<double> v1;
            Point<double> v2;
        };

        struct Hash {
            int operator()(const Key &k) const {
                return static_cast<int>(k.id1 + k.id2 + k.v1.x + k.v1.y + k.v2.x + k.v2.y);
            }
        };

        struct Equals {
            bool operator()(const Key &left, const Key &right) const {
                return left.id1 == right.id1 && left.id2 == right.id2 && left.v1 == right.v1 && left.v2 == right.v2;
            }
        };

        std::unordered_set<Key, Hash, Equals> previous;
        std::unordered_set<Key, Hash, Equals> next;
    public:
        void prepare();
        void add(const Object &a, const Object &b);
        bool test(const Object &a, const Object &b) const;
    };
}

#endif
