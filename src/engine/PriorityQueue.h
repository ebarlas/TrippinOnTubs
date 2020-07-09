#ifndef TRIPPIN_PRIORITYQUEUE_H
#define TRIPPIN_PRIORITYQUEUE_H

#include <queue>
#include <unordered_set>
#include <utility>

namespace trippin {
    // Priority queue class that records de-queued items and prevents repeat pops.
    // Internal priority queue is implemented using binary heap.
    // Popped items are recorded using a hash table.
    template<class V, class M, class C>
    class PriorityQueue {
    private:
        using Pair = std::pair<V, M>;
        std::unordered_set<V> visited{};
        std::priority_queue<Pair, std::vector<Pair>, C> q;
        void removeVisited();
    public:
        PriorityQueue(const C &compare);
        void push(V value);
        void push(V value, M material);
        V pop();
        bool empty();
    };
}

template<class V, class M, class C>
trippin::PriorityQueue<V, M, C>::PriorityQueue(const C &compare) : q(compare) {}

template<class V, class M, class C>
void trippin::PriorityQueue<V, M, C>::removeVisited() {
    while (!q.empty() && visited.count(q.top().first)) {
        q.pop();
    }
}

template<class V, class M, class C>
void trippin::PriorityQueue<V, M, C>::push(V value) {
    q.push({value, {}});
}

template<class V, class M, class C>
void trippin::PriorityQueue<V, M, C>::push(V value, M material) {
    if (visited.count(value) == 0) {
        q.push({value, material});
    }
}

template<class V, class M, class C>
V trippin::PriorityQueue<V, M, C>::pop() {
    removeVisited();
    auto top = q.top();
    q.pop();
    visited.insert(top.first);
    return top.first;
}

template<class V, class M, class C>
bool trippin::PriorityQueue<V, M, C>::empty() {
    removeVisited();
    return q.empty();
}

#endif