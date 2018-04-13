#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int light_up;
uniform bool flash;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec3 white = vec3(255, 255, 255);
	vec3 combined_color= fcolor * vcolor;
	if (flash) {
		combined_color = combined_color + white;
	}

	color = vec4(combined_color, 1.0);

	// float radius = distance(vec2(0.0), vpos);
	// if (light_up == 1 && radius < 1.0)
	// {
	// 	// 0.6 is just to make it not too strong
	// 	color.xyz += (1.0 - radius) * 0.6 * vec3(1.0, 1.0, 1.0);
	// }
}