#pragma once

#include "common.hpp"
#include <list>

//game screen toolbox
class Toolbox : public Renderable
{
    static Texture toolbox_texture;

public:

        // Creates all the associated render resources and default transform of toolbox
        bool init(vec2 position);

        // Releases all the associated resources
        void destroy();

        //update slots
        void update();

        void draw(const mat3& projection) override;     

private:
        vec2 m_position; // Window coordinates
	    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	    float m_rotation; // in radians


};