//
// Created by Mark Pedersen on 2018-03-06.
//

#include "Limb.h"
#include "viewHelper.hpp"
#include <iostream>


Texture Limb::leg_texture;
Texture Limb::arm_texture;

// current frame in animation
int curr_frame_limb = 0;
// frame to draw and previous frame in sprite
int sprite_frame_index_limb = 0;
// sprite information
int sprite_width_arm = 522;
int sprite_height_arm = 242;
int sprite_width_leg = 410;
int sprite_height_leg = 240;
int num_rows_limb = 1;
int num_cols_limb = 5;
int frames_arm [5] = {0, 1, 2, 3, 4};
// animation timing
int frame_time_limb = 100;
auto start_time_limb = std::chrono::high_resolution_clock::now();

bool Limb::init(std::string inputtype) {
    // Load shared texture
    type = inputtype;
    
    float wr;
    float hr;
    
    if(type == "arm") {
        if(!arm_texture.is_valid()) {
            if (!arm_texture.load_from_file(tools_textures_path("arm_sprite.png")))
            {
                fprintf(stderr, "Failed to load arms texture!");
                return false;
            }
        }
        wr = sprite_width_arm * 0.5f;
        hr = sprite_height_arm * 0.5f;
    } else {
        if (!leg_texture.load_from_file(tools_textures_path("leg_sprite.png")))
        {
            fprintf(stderr, "Failed to load leg texture!");
            return false;
        }
        wr = sprite_width_leg * 0.5f;
        hr = sprite_width_leg * 0.5f;
    }
    
    // The position corresponds to the center of the texture
    // float wr = limb_texture.width * 0.5f;
    // float hr = limb_texture.height * 0.5f;
    
    TexturedVertex vertices[4];
    vertices[0].position = { -wr, +hr, -0.02f };
    vertices[0].texcoord = { 1/5.f, 1.f };
    vertices[1].position = { +wr, +hr, -0.02f };
    vertices[1].texcoord = { 0.f, 1.f };
    vertices[2].position = { +wr, -hr, -0.02f };
    vertices[2].texcoord = { 0.f, 0.f };
    vertices[3].position = { -wr, -hr, -0.02f };
    vertices[3].texcoord = { 1/5.f, 0.f };
    
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
    m_scale.x = -0.12f * ViewHelper::getRatio();
    m_scale.y = 0.12f * ViewHelper::getRatio();;

    return true;
}

void Limb::draw(const mat3& projection)
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
    GLint vertexColorLocation = glGetUniformLocation(effect.program, "our_color");
    
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

    // color changing
    float timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    glUniform3f(vertexColorLocation, 0.0f, greenValue, 0.0f);

    // Specify uniform variables
    glUniform1iv(sprite_frame_index_uloc, 1, &sprite_frame_index_limb);
    glUniform1iv(num_rows_uloc, 1, &num_rows_limb);
    glUniform1iv(num_cols_uloc, 1, &num_cols_limb);
    
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
        return { std::fabs(m_scale.x) * sprite_width_arm, std::fabs(m_scale.y) * sprite_height_arm };
     } else {
        return { std::fabs(m_scale.x) * sprite_width_leg, std::fabs(m_scale.y) * sprite_height_leg };
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

vec2 &Limb::getCurrentTarget() {
    return cur_target;
}

vec2 &Limb::getLastTarget() {
    return last_target;
}

JPS::PathVector &Limb::getCurrentPath() {
    return currentPath;
}

JPS::PathVector &Limb::getLastPath() {
    return lastPath;
}

vec2 &Limb::get_position() {
    return m_position;
}

void Limb::set_position(vec2 position) {
    this->m_position = position;
}
 
void Limb::move(vec2 pos) {
    this->m_position += pos;
    animate();
}

void Limb::animate()
{
    auto curr_time = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time_limb).count();

    if (milliseconds > frame_time_limb)
    {
        curr_frame_limb = (curr_frame_limb + 1) % 5;
        sprite_frame_index_limb = frames_arm[curr_frame_limb];
        start_time_limb = curr_time;
    }
}

void Limb::initLegTime(){
    if (type == "leg")
        legTime = time(0);
}
time_t Limb::getLegTime(){
    if (type == "leg")
        return legTime;
    else {
        fprintf(stderr, "Limb::getLegTime of arm");
        return time(0);
    }
}

void Limb::on_player1_collision(Kinetic *player) {
    printf("player1");
}

void Limb::on_player2_collision(Kinetic *player) {
    printf("player2");
}

void Limb::on_antidote_collision(Kinetic *antidote) {
    printf("antidote");
}

void Limb::on_limb_collision(Kinetic *limb) {
    printf("limb");
}

void Limb::on_armour_collision(Kinetic *player) {
    printf("armour");
}

void Limb::on_explosion_collision(Kinetic *explosion) {
    printf("explosion");
}

void Limb::on_ice_collision(Kinetic *ice) {
    printf("ice");
}

void Limb::on_missile_collision(Kinetic *missile) {
    printf("missile");
}

void Limb::on_water_collision(Kinetic *water) {
    printf("water");
}

void Limb::on_zombie_collision(Kinetic *zombie) {
    printf("zombie");
}

