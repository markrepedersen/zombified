#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Antidote : public Renderable
{
    static Texture antidote_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen);
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    // True if the salmon is alive
    bool is_alive()const;
    
    void destroy();
    
    vec2 get_bounding_box()const;

    int belongs_to; // who is currently holding the antidote
    
private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

};