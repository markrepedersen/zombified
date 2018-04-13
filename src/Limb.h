//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_LIMB_H
#define ZOMBIE_LIMB_H

#include "JPS.h"
#include "common.hpp"
#include "Kinetic.h"
#include <time.h>

class Limb : public Kinetic {

static Texture leg_texture;
static Texture arm_texture;

public:
    bool init(std::string type);

    void draw(const mat3& projection)override;

    void destroy();

    vec2 get_bounding_box()const;
    
    void setLastPath(JPS::PathVector path);

    void setCurrentPath(JPS::PathVector path);

    void setCurrentTarget(vec2 target);

    void setLastTarget(vec2 target);

    vec2 &getCurrentTarget();

    vec2 &getLastTarget();

    JPS::PathVector &getCurrentPath();

    JPS::PathVector &getLastPath();

    vec2 &get_position();

    void set_position(vec2 position);

    void move(vec2 pos)override;

    std::string getLimbType();

    bool isInitialized() {return initialized;}

    void setInitialized(bool flag) {this->initialized = flag;}

    void animate();
    
    void initLegTime();
    time_t getLegTime();
    
    time_t legTime;

    void on_player1_collision(Kinetic *player)override;
    void on_player2_collision(Kinetic *player)override;
    void on_antidote_collision(Kinetic *antidote)override;
    void on_limb_collision(Kinetic *limb)override;
    void on_armour_collision(Kinetic *player)override;
    void on_explosion_collision(Kinetic *explosion)override;
    void on_ice_collision(Kinetic *ice)override;
    void on_missile_collision(Kinetic *missile)override;
    void on_water_collision(Kinetic *water)override;
    void on_zombie_collision(Kinetic *zombie)override;

    vec2 getAABB()override;

protected:
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
    JPS::PathVector currentPath;
    vec2 m_scale;
    std::string type;
    bool initialized = false;
};

#endif //ZOMBIE_LIMB_H
