#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class Mud : public Renderable
{
    static Texture mud_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);

    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    void destroy();
    
    vec2 get_bounding_box()const;
    
    bool is_affected(int player)const;
    void set_affected(int player, bool affected);
    
private:
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    bool affectedp1;
    bool affectedp2;
};