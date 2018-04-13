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
    
    static Texture winner1_texture;
    static Texture winner2_texture;
    
    static Texture key_texture;
    
    static Texture pause_texture;
    
public:
    // Creates all the associated render resources and default transform
    bool init(std::string Infotype);
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    void destroy();
    
    void scale();

    bool init_tool();
    bool init_freeze();
    bool init_water();
    bool init_mud();
    bool init_bomb();
    bool init_missile();
    bool init_armour();
    bool init_winner2();
    bool init_winner1();
    bool init_key();

private:
    vec2 m_position; // Window coordinates
    vec2 m_scale;
    std::string type;
};