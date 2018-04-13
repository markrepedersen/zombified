// Header
#include "Ice.hpp"
#include "viewHelper.hpp"

#include <cmath>

Texture Ice::ice_texture;

bool Ice::init()
{
    // Load shared texture
    if (!ice_texture.is_valid())
    {
        if (!ice_texture.load_from_file(tools_textures_path("freeze.png")))
        {
            fprintf(stderr, "Failed to load freeze texture!");
            return false;
        }
    }
    
    // The position corresponds to the center of the texture
    float wr = ice_texture.width * 0.5f;
    float hr = ice_texture.height * 0.5f;
    
    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 0.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { 1.f, 1.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { 1.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 0.f, 0.f };
    
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
    mass = 0.15;
    m_scale.x = -0.10f * ViewHelper::getRatio();
    m_scale.y = 0.10f * ViewHelper::getRatio();
    m_is_alive = true;
    m_position = { 450.f * ViewHelper::getRatio(), 450.f * ViewHelper::getRatio()};
    frozen = 0;
    
    return true;
}

void Ice::draw(const mat3& projection)
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
    glBindTexture(GL_TEXTURE_2D, ice_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Ice::set_position(vec2 position)
{
    m_position = position;
}

void Ice::set_scale(vec2 scale)
{
    m_scale = scale;
}

vec2 Ice::get_position()const
{
    return m_position;
}

float Ice::get_mass() const
{
    return mass;
}

bool Ice::is_alive()const
{
    return m_is_alive;
}

void Ice::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

vec2 Ice::get_bounding_box()const
{
    // fabs is to avoid negative scale due to the facing direction
    return { std::fabs(m_scale.x) * ice_texture.width, std::fabs(m_scale.y) * ice_texture.height };
}

bool Ice::collides_with(const Ice& freeze)
{
    float dx = m_position.x - freeze.get_position().x;
    float dy = m_position.y - freeze.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(freeze.get_bounding_box().x, freeze.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

int Ice::use_freeze(int useOnPlayer)
{
    return useOnPlayer;
}

void Ice::on_player1_collision(Kinetic *player) {
    printf("player1");
}

void Ice::on_player2_collision(Kinetic *player) {
    printf("player2");
}

void Ice::on_antidote_collision(Kinetic *antidote) {
    printf("antidote");
}

void Ice::on_limb_collision(Kinetic *limb) {
    printf("limb");
}

void Ice::on_armour_collision(Kinetic *player) {
    printf("armour");
}

void Ice::on_explosion_collision(Kinetic *explosion) {
    printf("explosion");
}

void Ice::on_ice_collision(Kinetic *ice) {
    printf("ice");
}

void Ice::on_missile_collision(Kinetic *missile) {
    printf("missile");
}

void Ice::on_water_collision(Kinetic *water) {
    printf("water");
}

void Ice::on_zombie_collision(Kinetic *zombie) {
    printf("zombie");
}

vec2 Ice::getAABB() {
    return {static_cast<float>(ice_texture.width), static_cast<float>(ice_texture.height)};
}