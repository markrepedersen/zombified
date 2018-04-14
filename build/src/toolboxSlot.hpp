#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include <list>

//game screen toolbox
class ToolboxSlot : public Renderable
{
    static Texture toolboxSlot_texture;

public:

        // Creates all the associated render resources and default transform of toolboxSlot
        bool init(float initialOffset, float offset, float index, float y, int itemIndex);

        // Releases all the associated resources
        void destroy();

        //update slot
        void update();

        void draw(const mat3& projection) override;

private:
        vec2 m_position; // Window coordinates
        vec2 m_scale_original; // the scale of the texture, before scaling by viewHelper's getRatio();
	    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture, already scaled by the viewHelper's getRatio();
	    float m_rotation; // in radians


};