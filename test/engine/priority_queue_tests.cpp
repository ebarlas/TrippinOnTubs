#include "catch2/catch.hpp"
#include "engine/Queue.h"

TEST_CASE("Queue lifecycle no material", "[queue]") {
    auto compare = [](const std::pair<int, bool> &left, const std::pair<int, bool> &right) {
        return left.first < right.first;
    };

    trippin::Queue<int, bool, decltype(compare)> q(compare);
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

    trippin::Queue<int, bool, decltype(compare)> q(compare);
    q.push(1);
    q.push(2);
    q.push(3);
    REQUIRE(q.size() == 3);
    REQUIRE(!q.empty());
    REQUIRE(!q.material(1));
    REQUIRE(!q.material(2));
    REQUIRE(!q.material(2));
    REQUIRE(q.pop() == 3);
    q.push(1, true);
    REQUIRE(q.material(1));
    REQUIRE(q.pop() == 1);
    REQUIRE(q.pop() == 2);
    REQUIRE(q.wasVisited(1));
    REQUIRE(q.wasVisited(2));
    REQUIRE(q.wasVisited(3));
    REQUIRE(q.size() == 0);
    REQUIRE(q.empty());
}