// Header
#include "button.hpp"

#include <cmath>

Texture Button::startbutton_texture;
Texture Button::infobutton_texture;
Texture Button::backbutton_texture;

bool Button::init(std::string buttontype)
{
    type = buttontype;
    float wr = 0;
    float hr = 0;
    
    if(type == "start") {
        if (!startbutton_texture.is_valid()) {
            if (!startbutton_texture.load_from_file(startworld_textures_path("startbutton unclick.png")))
            {
                fprintf(stderr, "Failed to load start button texture!");
                return false;
            }
            wr = startbutton_texture.width * 0.5f;
            hr = startbutton_texture.height * 0.5f;
            m_scale.x = -0.95f* ViewHelper::getRatio();
            m_scale.y = 0.95f* ViewHelper::getRatio();
            m_position = { 640.f* ViewHelper::getRatio(), 360.f* ViewHelper::getRatio() }; //360
        }
    }
    else if(type == "info") {
        if (!infobutton_texture.is_valid()) {
            if (!infobutton_texture.load_from_file(startworld_textures_path("infobutton.png")))
            {
                fprintf(stderr, "Failed to load info button texture!");
                return false;
            }
            wr = infobutton_texture.width * 0.5f;
            hr = infobutton_texture.height * 0.5f;
            m_scale.x = -0.45f* ViewHelper::getRatio();
            m_scale.y = 0.45f* ViewHelper::getRatio();
            m_position = { 1100.f* ViewHelper::getRatio(), 660.f* ViewHelper::getRatio() };
        }
    }
    else {
        if (!backbutton_texture.is_valid()) {
            if (!backbutton_texture.load_from_file(startworld_textures_path("backbutton.png")))
            {
                fprintf(stderr, "Failed to load back button texture!");
                return false;
            }
            wr = backbutton_texture.width * 0.5f;
            hr = backbutton_texture.height * 0.5f;
            m_scale.x = -0.75f* ViewHelper::getRatio();
            m_scale.y = 0.75f* ViewHelper::getRatio();
            m_position = { 100.f* ViewHelper::getRatio(), 70.f* ViewHelper::getRatio() };
        }
    }
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
    b_is_clicked = false;
    
    return true;
}

void Button::draw(const mat3& projection)
{
    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    /*if(type == "start") {
        m_scale.x = -0.95f* ViewHelper::getRatio();
        m_scale.y = 0.95f* ViewHelper::getRatio();
    }
    else if(type == "info") {
        m_scale.x = -0.45f* ViewHelper::getRatio();
        m_scale.y = 0.45f* ViewHelper::getRatio();
    }
    else {
        m_scale.x = -0.75f* ViewHelper::getRatio();
        m_scale.y = 0.75f* ViewHelper::getRatio();
    }*/

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
    if(type == "start")
        glBindTexture(GL_TEXTURE_2D, startbutton_texture.id);
    else if(type == "info")
        glBindTexture(GL_TEXTURE_2D, infobutton_texture.id);
    else
        glBindTexture(GL_TEXTURE_2D, backbutton_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Button::click()
{
    if(type == "start") {
        if(!startbutton_texture.load_from_file(startworld_textures_path("startbutton unclick.png")))
        {
            fprintf(stderr, "Failed to load start button texture!");
        }
    }
    /*else if(type == "info") {
        if (!infobutton_texture.load_from_file(startworld_textures_path("infobutton.png")))
        {
            fprintf(stderr, "Failed to load info button texture!");
        }
    }
    else {
        if (!backbutton_texture.load_from_file(startworld_textures_path("backbutton.png")))
        {
            fprintf(stderr, "Failed to load back button texture!");
        }
    }*/

    b_is_clicked = true;
}

void Button::clickicon()
{
    if(type == "start") {
        if(!startbutton_texture.load_from_file(startworld_textures_path("startbutton click.png")))
        {
            fprintf(stderr, "Failed to load start button clicked texture!");
        }
    }
    /*else if(type == "info") {
        if (!infobutton_texture.load_from_file(startworld_textures_path("infobutton.png")))
        {
            fprintf(stderr, "Failed to load info button texture!");
        }
    }
    else {
        if (!backbutton_texture.load_from_file(startworld_textures_path("backbutton.png")))
        {
            fprintf(stderr, "Failed to load back button texture!");
        }
    }*/
}

bool Button::is_clicked()const
{
    return b_is_clicked;
}

void Button::unclick()
{
    b_is_clicked = false;
}

void Button::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteProgram(effect.program);
    b_is_clicked = false;
}
