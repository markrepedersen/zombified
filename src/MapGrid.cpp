//
// Created by Mark Pedersen on 2018-02-24.
//
#include "MapGrid.h"

MapGrid::MapGrid(unsigned w, unsigned h) : width(w), height(h) {}

bool MapGrid::operator()(unsigned x, unsigned y) const {
    return x < width && y < height && this->mapdata[y][x].walkable;
}

bool MapGrid::findPath(std::vector<JPS::Position> path, vec2 src, vec2 dst) {
    return JPS::findPath(path, *this, src.x, src.y, dst.x, dst.y, 0);
}

