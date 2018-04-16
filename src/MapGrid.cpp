//
// Created by Mark Pedersen on 2018-02-24.
//
#include "MapGrid.h"

MapGrid::MapGrid(unsigned w, unsigned h) : width(w), height(h) {
    mapdata.resize(width);
    for (int i = 0; i < width; ++i) {
        mapdata[i].resize(height);
        for (int j = 0; j < height; ++j) {
            mapdata[i][j] = new Node(1, true, REGULAR);
        }
    }
}

bool MapGrid::operator()(unsigned x, unsigned y) const {
    return x < width && y < height;
}
