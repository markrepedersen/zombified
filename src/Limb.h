//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_LIMB_H
#define ZOMBIE_LIMB_H

#include "common.hpp"
#include "JPS.h"

class Limb {
public:
    void setLastNode(vec2 node) {
        this->lastNode = node;
    }

    void setCurrentNode(vec2 node) {
        this->curNode = node;
    }

    void setPath(JPS::PathVector path) {
        this->lastPath = path;
    }

    void setCurrentTarget(vec2 target) {
        this->cur_target = target;
    }

    void setLastTarget(vec2 target) {
        this->last_target = target;
    }

    vec2 getCurrentTarget()const {
        return cur_target;
    }

    vec2 getLastTarget()const {
        return last_target;
    }

    vec2 getLastNode()const {
        return lastNode;
    }

    vec2 getCurrentNode()const {
        return curNode;
    }

    JPS::PathVector getPath()const {
        return lastPath;
    }
protected:
    vec2 curNode;
    vec2 lastNode;
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
};

#endif //ZOMBIE_LIMB_H
