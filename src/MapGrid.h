//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_MAPGRID_H
#define ZOMBIE_MAPGRID_H

#include "common.hpp"
#include <vector>
#include "JPS.h"
enum Terrain {
    REGULAR = 1,
    POOP,
    WATER,
    ICE,
    OCCUPADO
};

struct Node {
    Node(int sexiness, bool walkable, int terrain) : sexiness(sexiness), walkable(walkable), terrain(terrain) {};
    int sexiness;
    bool walkable;
    int terrain;
};

class MapGrid {
public:
    MapGrid (unsigned, unsigned);
    bool operator()(unsigned x, unsigned y) const;
    void destroy();
private:
    const unsigned width, height;
    std::vector<std::vector<Node*>> mapdata;
};

#endif //ZOMBIE_MAPGRID_H
