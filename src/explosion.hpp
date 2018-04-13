#pragma once

#include "common.hpp"
#include "Kinetic.h"
//#include <math.h>

class Explosion : public Kinetic
{
    static Texture explosion_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 position);
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    void destroy();

    void animate();

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
	float m_rotation;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    bool end_animation;
};