#pragma once

#include "common.hpp"
#include "Kinetic.h"
#include <time.h>

class Mud : public Kinetic
{
    static Texture mud_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);

    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
    bool is_affected(int player)const;
    void set_affected(int player, bool affected);
    
    time_t mudTime;

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
    
private:
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    bool affectedp1;
    bool affectedp2;
};