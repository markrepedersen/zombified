// Header
#include "mud.hpp"

#include <cmath>

Texture Mud::mud_texture;

bool Mud::init()
{
    // Load shared texture
    if (!mud_texture.is_valid())
    {
        if (!mud_texture.load_from_file(tools_textures_path("mud.png")))
        {
            fprintf(stderr, "Failed to load mud texture!");
            return false;
        }
    }
    
    // The position corresponds to the center of the texture
    float wr = mud_texture.width * 0.5f;
    float hr = mud_texture.height * 0.5f;
    
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
    m_scale.x = -0.25f * ViewHelper::getRatio();
    m_scale.y = 0.25f * ViewHelper::getRatio();
    m_position = { 350.f * ViewHelper::getRatio(), 450.f* ViewHelper::getRatio() };
    affectedp1 = false;
    affectedp2 = false;
    return true;
}

void Mud::draw(const mat3& projection)
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
    glBindTexture(GL_TEXTURE_2D, mud_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Mud::set_position(vec2 position)
{
    m_position = position;
}

vec2 Mud::get_position()const
{
    return m_position;
}

void Mud::set_scale(vec2 scale)
{
    m_scale = scale;
}

bool Mud::is_affected(int player)const
{
    if (player ==1 )
        return affectedp1;
    else
        return affectedp2;
}
void Mud::set_affected(int player, bool affected)
{
    if (player ==1 )
        affectedp1 = affected;
    else
        affectedp2 = affected;
}

void Mud::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteProgram(effect.program);
}

vec2 Mud::get_bounding_box()const
{
    // fabs is to avoid negative scale due to the facing direction
    //fprintf(stderr, "texture height %f \n", (std::fabs(m_scale.y) * mud_texture.height)-20 );
    return { std::fabs(m_scale.x) * mud_texture.width, std::fabs(m_scale.y) * mud_texture.height };
}
