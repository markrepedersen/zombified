#version 330
// From vertex shader
in vec2 texcoord;
in vec4 vertexColor;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

uniform vec3 our_color;
uniform int currFrame;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
    ivec2 offset = ivec2(225, 0);
	// color = vec4(our_color, 1.0) * textureOffset(sampler0, vec2(texcoord.x, texcoord.y), ivec2(225, 0));

    // noise effect
    // color = color * texture(sampler0, vec2(texcoord.x, texcoord.y));

    color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
}
