#version 330
// From vertex shader
in vec2 texcoord;
in vec4 vertexColor;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

uniform vec3 effect_color;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
    color = vec4(effect_color, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
}
