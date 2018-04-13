#pragma once

#include "common.hpp"
#include "JPS.h"
#include "Kinetic.h"

class Zombie : public Kinetic
{
    static Texture zombie_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    void destroy();
    
    vec2 get_bounding_box()const;

    void setLastPath(JPS::PathVector path);

    void setCurrentPath(JPS::PathVector path);

    void setCurrentTarget(vec2 target);

    void setLastTarget(vec2 target);

    void setAttackTimeout(float timeout);

    void setMass(float mass);

    vec2 getCurrentTarget()const;

    vec2 getLastTarget()const;

    JPS::PathVector getCurrentPath()const;

    JPS::PathVector getLastPath()const;
    
    float getAttackTimeout();

    float getMass();

    void move(vec2 pos);

    bool isInitialized() {return initialized;}

    void setInitialized(bool flag) {this->initialized = flag;}

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

protected:
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
    JPS::PathVector currentPath;
    vec2 m_scale;
    bool initialized = false;
    float attack_timeout;
    float mass;
};