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

   // if (x < width && y < height && x > 0 && y > 0) return true;
    // else if()
    //check how many times point collides with list of lines


    return x < width && y < height;
}

void MapGrid::destroy() {
    mapdata.clear();
}