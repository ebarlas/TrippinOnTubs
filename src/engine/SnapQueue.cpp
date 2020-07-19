#include "engine/SnapQueue.h"

bool trippin::SnapQueue::Compare::operator()(const std::pair<Object *, Sides> &left,
                                             const std::pair<Object *, Sides> &right) {
    // 1. platforms are highest priority
    if (left.first->platform)
        return false;
    if (right.first->platform)
        return true;

    // 2. non-platforms with most collision sides is next highest priority
    auto lcnt = left.second.count();
    auto rcnt = right.second.count();
    if (lcnt != rcnt)
        return lcnt < rcnt;

    // 3. non-platforms with highest y position (lower on screen) is next highest priority
    return left.first->position.y < right.first->position.y;
}

trippin::SnapQueue::SnapQueue() : q(Compare{}) {}

void trippin::SnapQueue::removeVisited() {
    while (!q.empty() && q.top().first->queueVisited) {
        q.pop();
    }
}

void trippin::SnapQueue::push(Object *obj) {
    obj->queueVisited = false;
    q.push({obj, {}});
}

void trippin::SnapQueue::push(Object *obj, Sides sides) {
    if (!obj->queueVisited) {
        q.push({obj, sides});
    }
}

trippin::Object *trippin::SnapQueue::pop() {
    removeVisited();
    auto top = q.top();
    q.pop();
    top.first->queueVisited = true;
    return top.first;
}

bool trippin::SnapQueue::empty() {
    removeVisited();
    return q.empty();
}