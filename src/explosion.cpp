// Header
#include "explosion.hpp"
#include "viewHelper.hpp"
#include "player1.hpp"
#include "MapGrid.h"

#include <cmath>
#include <iostream>

Texture Explosion::explosion_texture;

// current frame in animation
int curr_frame_explosion = 0;
// frame to draw and previous frame in sprite
int sprite_frame_index_explosion = 0;
// sprite information
int sprite_width_explosion = 210;
int sprite_height_explosion = 210;
int num_rows_explosion = 1;
int num_cols_explosion = 9;
int frames_explosion [9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
// animation timing
int frame_time_explosion = 100.f;
auto start_time_explosion = std::chrono::high_resolution_clock::now();

bool Explosion::init(vec2 position)
{
    // Load shared texture
    if (!explosion_texture.is_valid())
    {
        if (!explosion_texture.load_from_file(effects_textures_path("explosion.png")))
        {
            fprintf(stderr, "Failed to load explosion texture!");
            return false;
        }
    }
    
    // The position corresponds to the center of the texture
    float wr = sprite_width_explosion * 0.5f;
    float hr = sprite_height_explosion * 0.5f;
    
    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 1/9.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { 0.f, 1.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { 0.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 1/9.f, 0.f };
    
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
    if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
        return false;
    
    // Setting initial values
    m_scale.x = -0.25f * ViewHelper::getRatio();
    m_scale.y = 0.25f * ViewHelper::getRatio();
    m_position = { position.x, position.y };

    std::cout << "Explosion created" << "\n";

    animate();
    
    return true;
}

void Explosion::draw(const mat3& projection)
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
    glBindTexture(GL_TEXTURE_2D, explosion_texture.id);

    // Specify uniform variables
    glUniform1iv(sprite_frame_index_uloc, 1, &sprite_frame_index_explosion);
    glUniform1iv(num_rows_uloc, 1, &num_rows_explosion);
    glUniform1iv(num_cols_uloc, 1, &num_cols_explosion);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Explosion::set_position(vec2 position)
{
    m_position = position;
}

vec2 Explosion::get_position()const
{
    return m_position;
}

void Explosion::set_scale(vec2 scale)
{
    m_scale = scale;
}

void Explosion::destroy()
{
    MapGrid::GetInstance()->removeOccupant(this);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void Explosion::animate() {
    auto curr_time = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time_explosion).count();

    if (milliseconds > frame_time_explosion)
    {
        curr_frame_explosion = (curr_frame_explosion + 1);
        sprite_frame_index_explosion = frames_explosion[curr_frame_explosion];
        start_time_explosion = curr_time;
    }

    if (curr_frame_explosion > 9) {
        destroy();
    }
}

void Explosion::on_player1_collision(Kinetic *player) {
    printf("player1");
}

void Explosion::on_player2_collision(Kinetic *player) {
    printf("player2");
}

void Explosion::on_antidote_collision(Kinetic *antidote) {
    printf("antidote");
}

void Explosion::on_limb_collision(Kinetic *limb) {
    printf("limb");
}

void Explosion::on_armour_collision(Kinetic *player) {
    printf("armour");
}

void Explosion::on_explosion_collision(Kinetic *explosion) {
    printf("explosion");
}

void Explosion::on_ice_collision(Kinetic *ice) {
    printf("ice");
}

void Explosion::on_missile_collision(Kinetic *missile) {
    printf("missile");
}

void Explosion::on_water_collision(Kinetic *water) {
    printf("water");
}

void Explosion::on_zombie_collision(Kinetic *zombie) {
    printf("zombie");
}

vec2 Explosion::getAABB() {
    return {static_cast<float>(explosion_texture.width), static_cast<float>(explosion_texture.height)};
}