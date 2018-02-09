#pragma once

#include "common.hpp"

class Worldtexture : public Renderable
{
    static Texture world_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen);
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    void destroy();
    
private:
    //bool b_is_clicked;
    vec2 m_position; // Window coordinates
    vec2 m_scale;
};