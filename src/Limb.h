//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_LIMB_H
#define ZOMBIE_LIMB_H

#include "JPS.h"
#include "common.hpp"

class Limb {
public:
    void setLastPath(JPS::PathVector path);

    void setCurrentPath(JPS::PathVector path);

    void setCurrentTarget(vec2 target);

    void setLastTarget(vec2 target);

    vec2 getCurrentTarget()const;

    vec2 getLastTarget()const;

    JPS::PathVector getCurrentPath()const;

    JPS::PathVector getLastPath()const;

    vec2 get_position()const;

    void set_position(vec2 position);

    void move(vec2 pos);
protected:
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
    JPS::PathVector currentPath;
    vec2 position;
};

#endif //ZOMBIE_LIMB_H
