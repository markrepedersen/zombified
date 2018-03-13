//
// Created by Mark Pedersen on 2018-03-12.
//

#ifndef ZOMBIE_COLLISIONMANAGER_H
#define ZOMBIE_COLLISIONMANAGER_H

#include <unordered_set>
#include "ToolManager.h"
#include "limbsManager.hpp"

class CollisionManager {
public:
    // Adds the object's position to the broadphase collision detection
    void addToBroadphase(vec2);

    // Adds the object's position to the broadphase collision detection and remove its last position if applicable
    void addToBroadphase(vec2, vec2);

    void processCollisions();

private:
    void getIdForObject();

    std::unordered_set<vec2> potentialCollisions;
};

#endif //ZOMBIE_COLLISIONMANAGER_H
