#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "toolboxManager.hpp"
#include <vector>
#include <tuple>
#include <unistd.h>

class Missile : public Renderable
{
    static Texture missile_texture;
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
    
    void set_speed(vec2 newspeed);
    vec2 get_speed()const;
    
    void set_rotation(float radians);
    
    // True if the salmon is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
    void move(vec2 pos);
    
    bool collides_with(const Missile& missile);
    
    float get_force(float mass, float speed, vec2 objPosition);

    bool checkPoint();
    
    int useMissileOnPlayer;
    vec2 onPlayerPos;
    
private:
    bool m_is_alive; // True if the salmon is alive
	float m_rotation;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	size_t m_num_indices; // passed to glDrawElements
    float mass;
    vec2 m_speed;
    vec2 m_position;
};