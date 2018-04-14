#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Button : public Renderable
{
    static Texture startbutton_texture;
    static Texture infobutton_texture;
    static Texture backbutton_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(std::string buttontype);
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    bool is_clicked()const;
    
    void click();
    void clickicon();
    void unclick();
    void destroy();
    
private:
    bool b_is_clicked;
    vec2 m_position; // Window coordinates
    vec2 m_scale;
    std::string type;
};