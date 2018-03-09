#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Tree : public Renderable
{
    static Texture tree_texture;
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
    
    float get_force(float mass, float speed, vec2 objPosition);
    
    //bool get_explode()const;
    //void set_explode();
    
    
private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    //bool explode;
};