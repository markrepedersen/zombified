#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "cstring"
#include "string"
//#include <math.h>

class Text2D : public Renderable
{
    static Texture font_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen, const char * text, vec2 position, int size);

    void update(const char * text, int size);

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

    std::vector<vec2> vertices;
    std::vector<vec2> UVs;

    float x;
    float y;
};