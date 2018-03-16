#version 330 
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

uniform int num_rows;
uniform int num_cols;
uniform int sprite_frame_index;

// uniform int currFrame;

void main()
{
	int row = sprite_frame_index / num_cols;
	int col = sprite_frame_index - row * num_cols;
	float tex_x = float(col) / float(num_cols);
	float tex_y = float(row) / float(num_rows);

	texcoord = in_texcoord + vec2(tex_x, tex_y);

	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}