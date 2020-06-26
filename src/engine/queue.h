#ifndef TRIPPIN_QUEUE_H
#define TRIPPIN_QUEUE_H

#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <utility>

namespace trippin {
    template<class V, class M, class C>
    class Queue {
    private:
        using Key = std::pair<V, M>;
        std::unordered_map<V, M> materials;
        std::unordered_set<V> visited{};
        std::priority_queue<Key, std::vector<Key>, C> q;
        void removeVisited();

    public:
        Queue(const C &compare);
        void push(V value);
        void push(V value, const M &material);
        V pop();
        M &material(V value);
        bool empty();
        std::size_t size();
        bool wasVisited(V value) const;
    };

    template<class V, class M, class C>
    Queue<V, M, C>::Queue(const C &compare) : q(compare) {}

    template<class V, class M, class C>
    void Queue<V, M, C>::removeVisited() {
        while (!q.empty() && visited.count(q.top().first)) {
            q.pop();
        }
    }

    template<class V, class M, class C>
    void Queue<V, M, C>::push(V value) {
        q.push({value, {}});
    }

    template<class V, class M, class C>
    void Queue<V, M, C>::push(V value, const M &material) {
        if (visited.count(value) == 0) {
            auto m = materials[value] |= material;
            q.push({value, m});
        }
    }

    template<class V, class M, class C>
    V Queue<V, M, C>::pop() {
        removeVisited();
        auto top = q.top();
        q.pop();
        visited.insert(top.first);
        return top.first;
    }

    template<class V, class M, class C>
    M &Queue<V, M, C>::material(V value) {
        return materials[value];
    }

    template<class V, class M, class C>
    bool Queue<V, M, C>::empty() {
        removeVisited();
        return q.empty();
    }

    template<class V, class M, class C>
    std::size_t Queue<V, M, C>::size() {
        removeVisited();
        return q.size();
    }

    template<class V, class M, class C>
    bool Queue<V, M, C>::wasVisited(V value) const {
        return visited.count(value);
    }
}

#endif