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
    
    // Renders the leg
    void draw(const mat3& projection)override;

    // Returns the current leg position
    vec2 get_position()const;

    // Sets the new leg position
    void set_position(vec2 position);
    
    // True if the leg is alive
    bool is_alive()const;

    void destroy();
    
    vec2 get_bounding_box()const;
    
private:
    bool m_is_alive; // True if the leg is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};