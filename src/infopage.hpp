#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Info : public Renderable
{
    static Texture toolpage_texture;
    static Texture freeze_texture;
    static Texture water_texture;
    static Texture mud_texture;
    static Texture bomb_texture;
    static Texture missile_texture;
    static Texture armour_texture;
    
public:
    // Creates all the associated render resources and default transform
    bool init(std::string Infotype);
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    void destroy();
    
    void scale();
    
private:
    vec2 m_position; // Window coordinates
    vec2 m_scale;
    std::string type;
};