// Header
#include "punchright.hpp"
#include "viewHelper.hpp"

#include <cmath>

Texture Punchright::punchright_texture;

bool Punchright::init(vec2 screen)
{
    
            if (!punchright_texture.load_from_file(tools_textures_path("gloveright.png")))
            {
                fprintf(stderr, "Failed to load punchright texture!");
                return false;
            }
            //fprintf(stderr, "printedright");
    
    // The position corresponds to the center of the texture
    float wr = punchright_texture.width * 0.5f;
    float hr = punchright_texture.height * 0.5f;
    
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
    m_scale.x = -0.06f * ViewHelper::getRatio();
    m_scale.y = 0.06f * ViewHelper::getRatio();
    m_is_alive = true;
    
    srand((unsigned)time(0));
    m_position = {screen.x/2 * ViewHelper::getRatio(), screen.y/2 * ViewHelper::getRatio()};
    
    belongs_to = 0;
    return true;
}

void Punchright::draw(const mat3& projection)
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
    glBindTexture(GL_TEXTURE_2D, punchright_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Punchright::set_position(vec2 position)
{
    m_position = position;
}

void Punchright::set_scale(vec2 scale)
{
    m_scale = scale;
}

vec2 Punchright::get_position()const
{
    return m_position;
}

bool Punchright::is_alive()const
{
    return m_is_alive;
}

void Punchright::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

vec2 Punchright::get_bounding_box()const
{
    // fabs is to avoid negative scale due to the facing direction
    return { std::fabs(m_scale.x) * punchright_texture.width+30.f, std::fabs(m_scale.y) * punchright_texture.height };
}

void Punchright::on_player1_collision(Kinetic *player) {
    printf("player1");
}

void Punchright::on_player2_collision(Kinetic *player) {
    printf("player2");
}

void Punchright::on_antidote_collision(Kinetic *antidote) {
    printf("antidote");
}

void Punchright::on_limb_collision(Kinetic *limb) {
    printf("limb");
}

void Punchright::on_armour_collision(Kinetic *player) {
    printf("armour");
}

void Punchright::on_explosion_collision(Kinetic *explosion) {
    printf("explosion");
}

void Punchright::on_ice_collision(Kinetic *ice) {
    printf("ice");
}

void Punchright::on_missile_collision(Kinetic *missile) {
    printf("missile");
}

void Punchright::on_water_collision(Kinetic *water) {
    printf("water");
}

void Punchright::on_zombie_collision(Kinetic *zombie) {
    printf("zombie");
}

vec2 Punchright::getAABB() {
    return {static_cast<float>(punchright_texture.width), static_cast<float>(punchright_texture.height)};
}