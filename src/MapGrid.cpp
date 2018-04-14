//
// Created by Mark Pedersen on 2018-02-24.
//
#include "MapGrid.h"

using namespace std;

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

bool MapGrid::isOccupied(int x, int y) const {
    int xScale = x / SCALING;
    int yScale = y / SCALING;
    return !mapdata[xScale > width-1 ? width-1 : xScale][yScale > height-1 ? height-1 : yScale]->getPopulation().empty();
}

void MapGrid::addOccupant(Kinetic *occupant) {
    vec2 dimensions = occupant->getAABB();
    auto xScale = (int) (occupant->m_position.x / SCALING);
    auto yScale = (int) (occupant->m_position.x / SCALING);
    auto wScale = dimensions.x / SCALING;
    auto hScale = dimensions.y / SCALING;
    AABB aabb(xScale, yScale, wScale, hScale);
    for (auto vec : aabb.getCornerPoints()) {
        mapdata[vec.x > width - 1 ? width - 1 : vec.x][vec.y > height - 1 ? height - 1 : vec.y]->addCollider(occupant);
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
        mapdata[vec.x > width - 1 ? width - 1 : vec.x][vec.y > height - 1 ? height - 1 : vec.y]->removeCollider(
                occupant);
    }
}

vector<Kinetic *> MapGrid::getPossibleColliders(int x, int y) {
    return mapdata[x][y]->getPopulation();
}

void MapGrid::processColliders() {
    for (int x = 0; x < this->width; ++x) {
        for (int y = 0; y < this->height; ++y) {
            vector<Kinetic*> colliders = getPossibleColliders(x, y);
            for (int i = 0; i < colliders.size(); ++i) {
                for (int j = 0; j < colliders.size(); ++j) {
                    if (colliders[i] != colliders[j]) {
                        AABB collider1(colliders[i]->m_position.x,
                                       colliders[i]->m_position.y,
                                       colliders[i]->getAABB().x,
                                       colliders[i]->getAABB().y);
                        AABB collider2(colliders[j]->m_position.x,
                                       colliders[j]->m_position.y,
                                       colliders[j]->getAABB().x,
                                       colliders[j]->getAABB().y);
                        if (is_aabb_colliding(collider1.x, collider1.y, collider1.width, collider1.height,
                                              collider2.x, collider2.y, collider2.width, collider2.height)) {
                            colliders[i]->collide(colliders[j]);
                            colliders[j]->collide(colliders[i]);
                        }
                    }
                }
            }
        }
    }
}