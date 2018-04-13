#pragma once

#include "common.hpp"
#include "Kinetic.h"
#include <vector>
#include <tuple>
#include <unistd.h>

class Armour : public Kinetic
{
    static Texture armour_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    float get_mass()const;
    
    // True if the salmon is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
    bool collides_with(const Armour& armour);

    vec2 getAABB()override;

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
    
private:
    bool m_is_alive; // True if the salmon is alive
	float m_rotation;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	size_t m_num_indices; // passed to glDrawElements
    float mass;
};