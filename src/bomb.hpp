#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "toolboxManager.hpp"
#include <vector>
#include <tuple>
#include <unistd.h>
//#include <math.h>

class Bomb : public Renderable
{
    static Texture bomb_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    vec2 get_speed()const;
    void set_speed(vec2 newSpeed);
    
    float get_mass()const;
    
    // True if the salmon is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
    float get_force(float mass, float speed, vec2 objPosition);
    
    bool collides_with(const Bomb& bomb);
    
    void move(vec2);
    
    void checkBoundaryCollision(float width, float height, float ms);
    void checkCollision(Bomb other, float ms);
    
private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
	float m_rotation;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	size_t m_num_indices; // passed to glDrawElements
    vec2 speed;
    float mass;
};