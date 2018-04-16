#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Lifep1 : public Renderable
{
    static Texture onequart_texture;
    static Texture twoquart_texture;
    static Texture threequart_texture;
    static Texture full_texture;
    
public:
    // Creates all the associated render resources and default transform
    bool init(std::string life);
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    void destroy();
    
private:
    vec2 m_position; // Window coordinates
    vec2 m_scale;
    std::string type;
};