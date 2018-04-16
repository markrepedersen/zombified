// Header
#include "mushroom_explosion.hpp"

#include <cmath>
#include <iostream>

Texture Mushroom_Explosion::mushroom_explosion_texture;

// sprite information
int sprite_width_mushroom_explosion = 320;
int sprite_height_mushroom_explosion = 320;
int num_rows_mushroom_explosion = 2;
int num_cols_mushroom_explosion = 8;
int frames_mushroom_explosion [16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
// animation timing
int frame_time_mushroom_explosion = 40.f;
auto start_time_mushroom_explosion = std::chrono::high_resolution_clock::now();

bool Mushroom_Explosion::init(vec2 position)
{
    // Load shared texture
    if (!mushroom_explosion_texture.is_valid())
    {
        if (!mushroom_explosion_texture.load_from_file(effects_textures_path("mushroom_cloud.png")))
        {
            fprintf(stderr, "Failed to load mushroom explosion texture!");
            return false;
        }
    }
    
    // The position corresponds to the center of the texture
    float wr = sprite_width_mushroom_explosion * 0.5f;
    float hr = sprite_height_mushroom_explosion * 0.5f;
    
    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 1/8.f, 1/2.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { 0.f, 1/2.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { 0.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 1/8.f, 0.f };
    
    // counterclockwise as it's the default opengl front winding direction
    uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };
    
    // Clearing errors
    gl_flush_errors();
    
    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);
    
    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
    
    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;
    
    // Loading shaders
    if (!effect.load_from_file(shader_path("player.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;
    
    // Setting initial values
    m_scale.x = -1.f * ViewHelper::getRatio();
    m_scale.y = 1.f * ViewHelper::getRatio();
    m_position = { position.x, position.y};

    // std::cout << "Explosion created" << "\n";
    end_animation = false;
    
    return true;
}

void Mushroom_Explosion::draw(const mat3& projection)
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
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLint num_rows_uloc = glGetUniformLocation(effect.program, "num_rows");
    GLint num_cols_uloc = glGetUniformLocation(effect.program, "num_cols");
    GLint sprite_frame_index_uloc = glGetUniformLocation(effect.program, "sprite_frame_index");
    
    
    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    
    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
    
    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mushroom_explosion_texture.id);

    // Specify uniform variables
    glUniform1iv(sprite_frame_index_uloc, 1, &sprite_frame_index_explosion);
    glUniform1iv(num_rows_uloc, 1, &num_rows_mushroom_explosion);
    glUniform1iv(num_cols_uloc, 1, &num_cols_mushroom_explosion);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    //std::cout << "Frame: " << sprite_frame_index_explosion << "\n";
    animate();
}

void Mushroom_Explosion::set_position(vec2 position)
{
    m_position = position;
}

vec2 Mushroom_Explosion::get_position()const
{
    return m_position;
}

void Mushroom_Explosion::set_scale(vec2 scale)
{
    m_scale = scale;
}

bool Mushroom_Explosion::get_end_animation()const
{
    return end_animation;
}

void Mushroom_Explosion::destroy()
{
    //printf("Destroy!\n");
    end_animation = true;
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void Mushroom_Explosion::animate() {
    auto curr_time = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time_mushroom_explosion).count();

    if (milliseconds > frame_time_mushroom_explosion)
    {
        curr_frame_explosion = (curr_frame_explosion + 1);
        sprite_frame_index_explosion = frames_mushroom_explosion[curr_frame_explosion];
        start_time_mushroom_explosion = curr_time;
    }

    if (curr_frame_explosion > 15) {
        destroy();
    }
}