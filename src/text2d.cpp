#include "text2d.hpp"
#include <vector>
#include <cstring>
#include <cmath>
#include <iostream>

Texture Text2D::font_texture;

bool Text2D::init(vec2 screen, const char * text, vec2 position, int size)
{
    x = position.x;
    y = position.y;

    // Load shared texture
    if (!font_texture.is_valid())
    {
        if (!font_texture.load_from_file(font_textures_path("font_texture.png")))
        {
            fprintf(stderr, "Failed to load font texture!");
            return false;
        }
    }

    unsigned int length = strlen(text);
    
	// Fill buffers
	for ( unsigned int i=0 ; i<length ; i++ ){
		
		vec2 vertex_up_left    = vec2( x+i*size     , y+size );
		vec2 vertex_up_right   = vec2( x+i*size+size, y+size );
		vec2 vertex_down_right = vec2( x+i*size+size, y      );
		vec2 vertex_down_left  = vec2( x+i*size     , y      );

		vertices.push_back(vertex_up_left   );
		vertices.push_back(vertex_down_left );
		vertices.push_back(vertex_up_right  );

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		vec2 uv_up_left    = vec2( uv_x           , uv_y );
		vec2 uv_up_right   = vec2( uv_x+1.0f/16.0f, uv_y );
		vec2 uv_down_right = vec2( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
		vec2 uv_down_left  = vec2( uv_x           , (uv_y + 1.0f/16.0f) );
		UVs.push_back(uv_up_left   );
		UVs.push_back(uv_down_left );
		UVs.push_back(uv_up_right  );

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}
    
    // Clearing errors
    gl_flush_errors();
    

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, UVs.size() * sizeof(vec2), &UVs[0], GL_STATIC_DRAW);
    
    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;
    
    // Loading shaders
    if (!effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl")))
        return false;
    
    // Setting initial values
    m_scale.x = -2.f * ViewHelper::getRatio();
    m_scale.y = 2.f * ViewHelper::getRatio();
    m_position = {position.x * ViewHelper::getRatio(), position.y * ViewHelper::getRatio()};
    
    return true;
}

void Text2D::update(const char * text, int size) {

    //std::cout << text << "\n";

    std::vector<vec2> vertices_update;
    std::vector<vec2> UVs_update;

    unsigned int length = strlen(text);
    
	// Fill buffers
	for ( unsigned int i=0 ; i<length ; i++ ){
		
		vec2 vertex_up_left    = vec2( x+i*size     , y+size );
		vec2 vertex_up_right   = vec2( x+i*size+size, y+size );
		vec2 vertex_down_right = vec2( x+i*size+size, y      );
		vec2 vertex_down_left  = vec2( x+i*size     , y      );

		vertices_update.push_back(vertex_up_left   );
		vertices_update.push_back(vertex_down_left );
		vertices_update.push_back(vertex_up_right  );

		vertices_update.push_back(vertex_down_right);
		vertices_update.push_back(vertex_up_right);
		vertices_update.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		vec2 uv_up_left    = vec2( uv_x           , uv_y );
		vec2 uv_up_right   = vec2( uv_x+1.0f/16.0f, uv_y );
		vec2 uv_down_right = vec2( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
		vec2 uv_down_left  = vec2( uv_x           , (uv_y + 1.0f/16.0f) );
		UVs_update.push_back(uv_up_left   );
		UVs_update.push_back(uv_down_left );
		UVs_update.push_back(uv_up_right  );

		UVs_update.push_back(uv_down_right);
		UVs_update.push_back(uv_up_right);
		UVs_update.push_back(uv_down_left);
	}
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_update.size() * sizeof(vec2), &vertices_update[0]);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, UVs_update.size() * sizeof(vec2), &UVs_update[0]);

    //std::cout << text << "\n";
}

void Text2D::draw(const mat3& projection)
{
    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    transform_begin();
    transform_translate(m_position);
    transform_scale(m_scale);
    transform_end();
    
    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    
	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_texture.id);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	// Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
    float color[] = {1.f, 1.f, 1.f};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.ibo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Text2D::set_position(vec2 position)
{
    m_position = position;
}

vec2 Text2D::get_position()const
{
    return m_position;
}

void Text2D::set_scale(vec2 scale)
{
    m_scale = scale;
}

void Text2D::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteProgram(effect.program);
}