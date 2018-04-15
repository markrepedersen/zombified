#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "toolboxManager.hpp"
#include <vector>
#include <tuple>
#include <unistd.h>

class Armour : public Renderable
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
    
private:
    bool m_is_alive; // True if the salmon is alive
	float m_rotation;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	size_t m_num_indices; // passed to glDrawElements
    float mass;
};