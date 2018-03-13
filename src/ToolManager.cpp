//
// Created by Mark Pedersen on 2018-03-12.
//

#include "ToolManager.h"

void ToolManager::draw_tools(const mat3 &projection_2D) {
    for (auto& ice : vIce) {
        ice.draw(projection_2D);
    }
    for (auto& water : vWater) {
        water.draw(projection_2D);
    }
}

void ToolManager::process_collisions() {

}
