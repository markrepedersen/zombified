//
// Created by Mark Pedersen on 2018-03-12.
//

#ifndef ZOMBIE_TOOLSMANAGER_H
#define ZOMBIE_TOOLSMANAGER_H


#include <vector>
#include "Ice.hpp"
#include "water.hpp"

class ToolManager {
public:
    void draw_tools(const mat3 &projection_2D);

    void process_collisions();

private:
    std::vector<Ice> vIce;
    std::vector<Water> vWater;
    vec2 screen;
};


#endif //ZOMBIE_TOOLSMANAGER_H
