//
// Created by Mark Pedersen on 2018-02-24.
//
#include "MapGrid.h"

MapGrid *MapGrid::instance = nullptr;

void MapGrid::Init(int x, int y) {
    int xScale = x / SCALING;
    int yScale = y / SCALING;
    width = xScale;
    height = yScale;
    mapdata.resize(xScale);
    for (int i = 0; i < xScale; ++i) {
        mapdata[i].resize(yScale);
        for (int j = 0; j < yScale; ++j) {
            mapdata[i][j] = new Node(1, true, REGULAR);
        }
    }
}

bool MapGrid::operator()(unsigned x, unsigned y) const {
    int xScale = x / SCALING;
    int yScale = y / SCALING;
    return xScale < width && yScale < height && !isOccupied(xScale, yScale);
}

bool MapGrid::isOccupied(int x, int y)const  {
    int xScale = x / SCALING;
    int yScale = y / SCALING;
    return !mapdata[xScale][yScale]->getPopulation().empty();
}

void MapGrid::addOccupant(Kinetic *occupant) {
    vec2 dimensions = occupant->getAABB();
    auto xScale = (int) (occupant->m_position.x / SCALING);
    auto yScale = (int) (occupant->m_position.x / SCALING);
    auto wScale = dimensions.x / SCALING;
    auto hScale = dimensions.y / SCALING;
    AABB aabb(xScale, yScale, wScale, hScale);
    for (auto vec : aabb.getCornerPoints()) {
        mapdata[vec.x > width-1 ? width-1 : vec.x][vec.y > height-1 ? height-1 : vec.y]->addCollider(occupant);
    }
}

void MapGrid::removeOccupant(Kinetic *occupant) {
    vec2 dimensions = occupant->getAABB();
    auto xScale = (int) (occupant->m_position.x / SCALING);
    auto yScale = (int) (occupant->m_position.x / SCALING);
    auto wScale = dimensions.x / SCALING;
    auto hScale = dimensions.y / SCALING;
    AABB aabb(xScale, yScale, wScale, hScale);
    for (auto vec : aabb.getCornerPoints()) {
        mapdata[vec.x > width-1 ? width-1 : vec.x][vec.y > height-1 ? height-1 : vec.y]->removeCollider(occupant);
    }
}