#include "Grid.h"

void trippin::Grid::setSize(Point<int> grid, Point<int> cell) {
    gridSize = grid;
    cellSize = cell;
    partitions.resize(grid.x * grid.y + 1);
}

trippin::Point<int> trippin::Grid::partition(Point<int> p) const {
    // grid may consist of only an overflow partition
    if (!cellSize || !gridSize) {
        return {-1, -1};
    }

    int x = p.x / cellSize.x;
    int y = p.y / cellSize.y;
    return {x >= 0 && x < gridSize.x ? x : -1, y >= 0 && y < gridSize.y ? y : -1};
}

void trippin::Grid::add(Object *obj) {
    // obtain upper left and lower right grid positions (col, row)
    auto upperLeft = partition(obj->roundedPosition);
    auto lowerRight = partition(obj->roundedPosition + obj->size);

    // account for positions that are off-grid
    Point<int> xRange{
            upperLeft.x >= 0 ? upperLeft.x : 0,
            lowerRight.x >= 0 ? lowerRight.x : gridSize.x - 1};
    Point<int> yRange{
            upperLeft.y >= 0 ? upperLeft.y : 0,
            lowerRight.y >= 0 ? lowerRight.y : gridSize.y - 1};

    // if any on-grid exposure, walk cells
    if (upperLeft.x >= 0 || upperLeft.y >= 0 || lowerRight.x >= 0 || lowerRight.y >= 0) {
        for (int x = xRange.x; x <= xRange.y; x++) {
            for (int y = yRange.x; y <= yRange.y; y++) {
                auto index = y * gridSize.x + x;
                auto &p = partitions[index];
                p.add(obj);
                obj->partitions.push_back(&p);
            }
        }
    }

    // if any off-grid exposure, include overflow partition
    if (upperLeft.x < 0 || upperLeft.y < 0 || lowerRight.x < 0 || lowerRight.y < 0) {
        auto &p = partitions[partitions.size() - 1];
        p.add(obj);
        obj->partitions.push_back(&p);
    }
}

void trippin::Grid::update(Object *obj) {
    // capture previous obj partitions
    auto prev = obj->partitions;
    obj->partitions.clear();

    // examine each object corner, add object to each partition and add each partition to object
    add(obj);

    // examine previous partitions, remove object from any exclusively former partition
    for (auto partition : prev) {
        if (std::find(obj->partitions.begin(), obj->partitions.end(), partition) == obj->partitions.end()) {
            partition->remove(obj);
        }
    }
}
