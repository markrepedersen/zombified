//
// Created by Mark Pedersen on 2018-03-12.
//

#include "CollisionManager.h"

void CollisionManager::addToBroadphase(vec2 currentPos) {
    potentialCollisions.insert(currentPos);
}

void CollisionManager::addToBroadphase(vec2 currentPos, vec2 lastPos) {
    potentialCollisions.insert(currentPos);
}

void CollisionManager::processCollisions() {

}