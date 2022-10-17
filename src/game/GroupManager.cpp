#include <algorithm>
#include "GroupManager.h"

void trippin::GroupManager::add(int group, int member) {
    groupMembers[group].push_back(member);
    groupSizes[group]++;
}

void trippin::GroupManager::remove(int group, int member) {
    auto &vec = groupMembers[group];
    vec.erase(std::remove(vec.begin(), vec.end(), member), vec.end());
}

double trippin::GroupManager::remaining(int group) {
    // with <= 1 member remaining, this ought to return 0.0
    return std::max(0.0, static_cast<double>(groupMembers[group].size()) - 1) / groupSizes[group];
}

bool trippin::GroupManager::empty(int group) {
    return groupMembers[group].empty();
}

int trippin::GroupManager::size(int group) {
    return groupSizes[group];
}
