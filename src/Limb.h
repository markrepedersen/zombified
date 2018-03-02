//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_LIMB_H
#define ZOMBIE_LIMB_H

#include "JPS.h"

class Limb {
public:
    void setLastPath(JPS::PathVector path) {
        this->lastPath = path;
    }

    void setCurrentPath(JPS::PathVector path) {
        this->currentPath = path;
    }

    void setCurrentTarget(vec2 target) {
        if(this->cur_target != NULL) {
            this->last_target = this->cur_target;
        }
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

    JPS::PathVector getCurrentPath()const {
        return currentPath;
    }

    JPS::PathVector getLastPath()const {
        return lastPath;
    }
protected:
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
    JPS::PathVector currentPath;
};

#endif //ZOMBIE_LIMB_H
