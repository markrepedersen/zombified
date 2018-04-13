#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "bomb.hpp"
#include "Kinetic.h"

class Tree : public Kinetic
{
    static Texture tree_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen);
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    // True if the salmon is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
    float get_force(float mass, float speed, vec2 objPosition);

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
    
    //bool get_explode()const;
    //void set_explode();

    vec2 getAABB()override;


private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    //bool explode;
};