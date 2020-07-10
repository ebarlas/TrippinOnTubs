#include "catch2/catch.hpp"
#include "engine/PriorityQueue.h"

TEST_CASE("Queue lifecycle no material", "[queue]") {
    auto compare = [](const std::pair<int, bool> &left, const std::pair<int, bool> &right) {
        return left.first < right.first;
    };

    trippin::PriorityQueue<int, bool, decltype(compare)> q(compare);
    q.push(1);
    q.push(2);
    q.push(3);
    REQUIRE(q.pop() == 3);
    REQUIRE(q.pop() == 2);
    REQUIRE(q.pop() == 1);
}

TEST_CASE("Queue lifecycle with material", "[queue]") {
    auto compare = [](const std::pair<int, bool> &left, const std::pair<int, bool> &right) {
        if (left.second != right.second) {
            return right.second;
        }

        return left.first < right.first;
    };

    trippin::PriorityQueue<int, bool, decltype(compare)> q(compare);
    q.push(1);
    q.push(2);
    q.push(3);
    REQUIRE(!q.empty());
    REQUIRE(q.pop() == 3);
    q.push(1, true);
    REQUIRE(q.pop() == 1);
    REQUIRE(q.pop() == 2);
    REQUIRE(q.empty());
}