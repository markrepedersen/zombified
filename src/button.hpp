#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Button : public Renderable
{
    static Texture button_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    bool is_clicked()const;
    
    void click();
    void clickicon();
    void destroy();
    
private:
    bool b_is_clicked;
    vec2 m_position; // Window coordinates
    vec2 m_scale;
};