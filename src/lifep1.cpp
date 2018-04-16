 // Header
#include "lifep1.hpp"

#include <cmath>

Texture Lifep1::onequart_texture;
Texture Lifep1::twoquart_texture;
Texture Lifep1::threequart_texture;
Texture Lifep1::full_texture;

bool Lifep1::init(std::string life)
{
    // Load shared texture
    //if (!Info_texture.is_valid())
    //{
    type = life;
    float wr = 0;
    float hr = 0;
    
    if(type == "one") {
        if (!onequart_texture.is_valid()) {
            //printf("1\n");
            if (!onequart_texture.load_from_file(effects_textures_path("life1.png")))
            {
                //printf("2\n");
                fprintf(stderr, "Failed to load life1 texture!");
                return false;
            }
            //printf("3\n");
            wr = onequart_texture.width * 0.5f;
            hr = onequart_texture.height * 0.5f;
            //printf("4\n");
        }
    }
    else if(type == "two") {
        if (!twoquart_texture.is_valid()) {
            if (!twoquart_texture.load_from_file(effects_textures_path("life2.png")))
            {
                fprintf(stderr, "Failed to load freeze details texture!");
                return false;
            }
            wr = twoquart_texture.width * 0.5f;
            hr = twoquart_texture.height * 0.5f;
        }
    }
    else if(type == "three") {
        if (!threequart_texture.is_valid()) {
            if (!threequart_texture.load_from_file(effects_textures_path("life3.png")))
            {
                fprintf(stderr, "Failed to load freeze details texture!");
                return false;
            }
            wr = threequart_texture.width * 0.5f;
            hr = threequart_texture.height * 0.5f;
        }
    }
    else if(type == "four") {
        if (!full_texture.is_valid()) {
            if (!full_texture.load_from_file(effects_textures_path("life4.png")))
            {
                fprintf(stderr, "Failed to load freeze details texture!");
                return false;
            }
            wr = full_texture.width * 0.5f;
            hr = full_texture.height * 0.5f;
        }
    }
    
        m_scale.x = -0.30f * ViewHelper::getRatio();
        m_scale.y = 0.30f * ViewHelper::getRatio();
        m_position = { 120.f* ViewHelper::getRatio(), 540.f* ViewHelper::getRatio() };

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
    
    return true;
}

void Lifep1::draw(const mat3& projection)
{
    //scale();
    // Transformation code, see Rendering and Transformation in the template specification for more Life
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
    
    if (type == "one")
        glBindTexture(GL_TEXTURE_2D, onequart_texture.id);
    else if(type == "two")
        glBindTexture(GL_TEXTURE_2D, twoquart_texture.id);
    else if(type == "three")
        glBindTexture(GL_TEXTURE_2D, threequart_texture.id);
    else if(type == "four")
        glBindTexture(GL_TEXTURE_2D, full_texture.id);
    
    
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Lifep1::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteProgram(effect.program);
}
