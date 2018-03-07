#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "Limb.h"

class Arms : public Renderable, public Limb
{
    static Texture arms_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the arms
    void draw(const mat3& projection)override;
    
    // True if the arm is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;
private:
    bool m_is_alive; // True if the arm is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};