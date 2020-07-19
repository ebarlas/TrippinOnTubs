#ifndef TRIPPIN_GRID_H
#define TRIPPIN_GRID_H

#include <vector>
#include <unordered_set>
#include "engine/Partition.h"
#include "engine/Object.h"

namespace trippin {
    struct Grid {
        // The overall size of the grid. That is, columns (x) and rows (y).
        Point<int> gridSize{};

        // The uniform size of each cell in the grid. That is, width (x) and height (y).
        Point<int> cellSize{};

        // The individual cells or partitions in the grid. Length is gridSize x * y + 1 for overflow.
        std::vector<Partition> partitions{1};

        Point<int> partition(Point<int> p) const;

        void setSize(Point<int> gridSize, Point<int> cellSize);
        void add(Object *obj);
        void update(Object *obj);
    };
}

#endif
