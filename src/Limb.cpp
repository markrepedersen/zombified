//
// Created by Mark Pedersen on 2018-03-06.
//

#include "Limb.h"

void Limb::setLastPath(JPS::PathVector path) {
    this->lastPath = path;
}

void Limb::setCurrentPath(JPS::PathVector path) {
    this->currentPath = path;
}

void Limb::setCurrentTarget(vec2 target) {
    if(this->cur_target != 0) {
        this->last_target = this->cur_target;
    }
    this->cur_target = target;
}

void Limb::setLastTarget(vec2 target) {
    this->last_target = target;
}

vec2 Limb::getCurrentTarget()const {
    return cur_target;
}

vec2 Limb::getLastTarget()const {
    return last_target;
}

JPS::PathVector Limb::getCurrentPath()const {
    return currentPath;
}

JPS::PathVector Limb::getLastPath()const {
    return lastPath;
}

vec2 Limb::get_position()const {
    return position;
}

void Limb::set_position(vec2 position) {
    this->position = position;
}

void Limb::move(vec2 pos) {
    position += pos;
}