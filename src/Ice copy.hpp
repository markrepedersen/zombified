#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include <vector>
#include <tuple>
#include <unistd.h>

class Instrucpage : public Renderable
{
    static Texture tools_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;
    
    void destroy();


private:
    bool m_position; // True if the salmon is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    

};