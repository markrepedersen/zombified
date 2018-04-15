#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
//#include <math.h>

class Blood : public Renderable
{
    static Texture blood_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 position);
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    void destroy();
    
private:
	//float m_rotation;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};