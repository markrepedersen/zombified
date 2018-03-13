//
// Created by Mark Pedersen on 2018-03-06.
//

#include "Limb.h"
#include <iostream>


Texture Limb::leg_texture;
Texture Limb::arm_texture;


bool Limb::init(std::string inputtype) {
        // Load shared texture
        type = inputtype;

        float wr;
        float hr;

        if(type == "arm") {
            if(!arm_texture.is_valid()) {
                if (!arm_texture.load_from_file(tools_textures_path("zombie arm.png")))
                {
                    fprintf(stderr, "Failed to load arms texture!");
                    return false; 
                }
            }
            wr = arm_texture.width * 0.5f;
            hr = arm_texture.height * 0.5f;
        } else {
            if (!leg_texture.load_from_file(tools_textures_path("zombie leg.png")))
            {
                fprintf(stderr, "Failed to load leg texture!");
                return false;
            }
            wr = leg_texture.width * 0.5f;
            hr = leg_texture.height * 0.5f;
        } 
    
    // // The position corresponds to the center of the texture
    // float wr = limb_texture.width * 0.5f;
    // float hr = limb_texture.height * 0.5f;
    
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
    m_scale.x = -0.12f * ViewHelper::getRatio();
    m_scale.y = 0.12f * ViewHelper::getRatio();;

    return true;
}

void Limb::draw(const mat3& projection)
{
     // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    transform_begin();
    transform_translate(position);
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
    if (type == "arm") {
        glBindTexture(GL_TEXTURE_2D, arm_texture.id);
    } else {
        glBindTexture(GL_TEXTURE_2D, leg_texture.id);
    }
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Limb::destroy()
{
        glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

vec2 Limb::get_bounding_box()const {
     // fabs is to avoid negative scale due to the facing direction
     if (type == "arm") {
        return { std::fabs(m_scale.x) * arm_texture.width, std::fabs(m_scale.y) * arm_texture.height };
     } else {
        return { std::fabs(m_scale.x) * leg_texture.width, std::fabs(m_scale.y) * leg_texture.height };
     }
}
    
std::string Limb::getLimbType() {
    return type;
}

void Limb::setLastPath(JPS::PathVector path) {
    this->lastPath = path;
}

void Limb::setCurrentPath(JPS::PathVector path) {
    this->currentPath = path;
}

void Limb::setCurrentTarget(vec2 target) {
    if(this->cur_target != 0) {
        this->last_target = this->cur_target;
    }
    this->cur_target = target;
}

void Limb::setLastTarget(vec2 target) {
    this->last_target = target;
}

vec2 Limb::getCurrentTarget()const {
    return cur_target;
}

vec2 Limb::getLastTarget()const {
    return last_target;
}

JPS::PathVector Limb::getCurrentPath()const {
    return currentPath;
}

JPS::PathVector Limb::getLastPath()const {
    return lastPath;
}

vec2 Limb::get_position()const {
    return position;
}

void Limb::set_position(vec2 position) {
    this->position = position;
}
 
void Limb::move(vec2 pos) {
    this->position += pos;
}