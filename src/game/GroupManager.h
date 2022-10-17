#ifndef TRIPPIN_GROUPMANAGER_H
#define TRIPPIN_GROUPMANAGER_H

#include <unordered_map>
#include <vector>

namespace trippin {
    class GroupManager {
    public:
        void add(int group, int member);
        void remove(int group, int member);
        double remaining(int group);
        bool empty(int group);
        int size(int group);
    private:
        std::unordered_map<int, int> groupSizes;
        std::unordered_map<int, std::vector<int>> groupMembers;
    };
}

#endif
