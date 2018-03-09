#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "Limb.h"

class Legs : public Renderable, public Limb
{
    static Texture legs_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    // Returns the current salmon position
    vec2 get_position()const;

    // Sets the new turtle position
    void set_position(vec2 position);
    
    void set_scale(vec2 scale);
    
    float get_mass()const;
    
    // True if the salmon is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    float mass;
};