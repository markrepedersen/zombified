 // Header
#include "infopage.hpp"

#include <cmath>

Texture Info::toolpage_texture;
Texture Info::freeze_texture;
Texture Info::water_texture;
Texture Info::mud_texture;
Texture Info::bomb_texture;
Texture Info::missile_texture;
Texture Info::armour_texture;

Texture Info::winner1_texture;
Texture Info::winner2_texture;
Texture Info::key_texture;
Texture Info::story_texture;

Texture Info::gameover_texture;
Texture Info::pause_texture;

bool Info::init(std::string infotype)
{
    // Load shared texture
    //if (!Info_texture.is_valid())
    //{
    type = infotype;
    float wr = 0;
    float hr = 0;
    
    if(type == "tool") {
        if (!toolpage_texture.is_valid()) {
            //printf("1\n");
            if (!toolpage_texture.load_from_file(startworld_textures_path("tooldefault.png")))
            {
                //printf("2\n");
                fprintf(stderr, "Failed to load tool page texture!");
                return false;
            }
            //printf("3\n");
            wr = toolpage_texture.width * 0.5f;
            hr = toolpage_texture.height * 0.5f;
            m_scale.x = -0.12f * ViewHelper::getRatio();
            m_scale.y = 0.12f * ViewHelper::getRatio();
            m_position = { 640.f* ViewHelper::getRatio(), 360.f* ViewHelper::getRatio() };
            //printf("4\n");
        }
    }
    else if(type == "freeze") {
        if (!freeze_texture.is_valid()) {
            if (!freeze_texture.load_from_file(startworld_textures_path("freeze text.png")))
            {
                fprintf(stderr, "Failed to load freeze details texture!");
                return false;
            }
            wr = freeze_texture.width * 0.5f;
            hr = freeze_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 430.f* ViewHelper::getRatio(), 150.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "water") {
        if (!water_texture.is_valid()) {
            if (!water_texture.load_from_file(startworld_textures_path("water text.png")))
            {
                fprintf(stderr, "Failed to load water details texture!");
                return false;
            }
            wr = water_texture.width * 0.5f;
            hr = water_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 430.f* ViewHelper::getRatio(), 365.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "mud") {
        if (!mud_texture.is_valid()) {
            if (!mud_texture.load_from_file(startworld_textures_path("mud text.png")))
            {
                fprintf(stderr, "Failed to load mud details texture!");
                return false;
            }
            wr = mud_texture.width * 0.5f;
            hr = mud_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 430.f* ViewHelper::getRatio(), 580.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "bomb") {
        if (!bomb_texture.is_valid()) {
            if (!bomb_texture.load_from_file(startworld_textures_path("bomb text.png")))
            {
                fprintf(stderr, "Failed to load bomb details texture!");
                return false;
            }
            wr = bomb_texture.width * 0.5f;
            hr = bomb_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 935.f* ViewHelper::getRatio(), 150.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "missile") {
        if (!missile_texture.is_valid()) {
            if (!missile_texture.load_from_file(startworld_textures_path("missile text.png")))
            {
                fprintf(stderr, "Failed to load missile details texture!");
                return false;
            }
            wr = missile_texture.width * 0.5f;
            hr = missile_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 935.f* ViewHelper::getRatio(), 365.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "armour") {
        if (!armour_texture.is_valid()) {
            if (!armour_texture.load_from_file(startworld_textures_path("armour text.png")))
            {
                fprintf(stderr, "Failed to load armour details texture!");
                return false;
            }
            wr = armour_texture.width * 0.5f;
            hr = armour_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 935.f* ViewHelper::getRatio(), 580.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "winner1") {
        if (!winner1_texture.is_valid()) {
            if (!winner1_texture.load_from_file(startworld_textures_path("winner1.png")))
            {
                fprintf(stderr, "Failed to load armour details texture!");
                return false;
            }
            wr = winner1_texture.width * 0.5f;
            hr = winner1_texture.height * 0.5f;
            m_scale.x = -0.15f * ViewHelper::getRatio();
            m_scale.y = 0.15f * ViewHelper::getRatio();
            m_position = { 250.f* ViewHelper::getRatio(), 150.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "winner2") {
        if (!winner2_texture.is_valid()) {
            if (!winner2_texture.load_from_file(startworld_textures_path("winner2.png")))
            {
                fprintf(stderr, "Failed to load armour details texture!");
                return false;
            }
            wr = winner2_texture.width * 0.5f;
            hr = winner2_texture.height * 0.5f;
            m_scale.x = -0.15f * ViewHelper::getRatio();
            m_scale.y = 0.15f * ViewHelper::getRatio();
            m_position = { 250.f* ViewHelper::getRatio(), 150.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "key") {
        if (!key_texture.is_valid()) {
            if (!key_texture.load_from_file(startworld_textures_path("keys.png")))
            {
                fprintf(stderr, "Failed to load armour details texture!");
                return false;
            }
            wr = key_texture.width * 0.5f;
            hr = key_texture.height * 0.5f;
            m_scale.x = -0.14f * ViewHelper::getRatio();
            m_scale.y = 0.14f * ViewHelper::getRatio();
            m_position = { 320.f* ViewHelper::getRatio(), 550.f* ViewHelper::getRatio() };
        }
    }
    else if(type == "pause") {
        if (!pause_texture.is_valid()) {
            if (!pause_texture.load_from_file(startworld_textures_path("pause.png")))
            {
                fprintf(stderr, "Failed to load pause texture!");
                return false;
            }
            wr = pause_texture.width * 0.5f;
            hr = pause_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 640.f* ViewHelper::getRatio(), 360.f* ViewHelper::getRatio() };
        }
    }
    
    else if(type == "story") {
        if (!story_texture.is_valid()) {
            if (!story_texture.load_from_file(startworld_textures_path("story.png")))
            {
                fprintf(stderr, "Failed to load story texture!");
                return false;
            }
            wr = story_texture.width * 0.5f;
            hr = story_texture.height * 0.5f;
            m_scale.x = -0.21f * ViewHelper::getRatio();
            m_scale.y = 0.21f * ViewHelper::getRatio();
            m_position = { 875.f* ViewHelper::getRatio(), 310.f* ViewHelper::getRatio() };
        }
    } else if(type == "gameover") {
        if (!gameover_texture.is_valid()) {
            // std::cout<<"render game over start"<< std::endl;
            if (!gameover_texture.load_from_file(startworld_textures_path("gameover1.png")))
            {
                fprintf(stderr, "Failed to load gameover texture!");
                return false;
            }
            wr = gameover_texture.width * 0.5f;
            hr = gameover_texture.height * 0.5f;
            m_scale.x = -0.25f * ViewHelper::getRatio();
            m_scale.y = 0.25f * ViewHelper::getRatio();
            m_position = { 630.f* ViewHelper::getRatio(), 300.f* ViewHelper::getRatio() };
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
    
    return true;
}

void Info::draw(const mat3& projection)
{
    //scale();
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
    
    if (type == "tool")
        glBindTexture(GL_TEXTURE_2D, toolpage_texture.id);
    else if(type == "freeze")
        glBindTexture(GL_TEXTURE_2D, freeze_texture.id);
    else if(type == "water")
        glBindTexture(GL_TEXTURE_2D, water_texture.id);
    else if(type == "mud")
        glBindTexture(GL_TEXTURE_2D, mud_texture.id);
    else if(type == "bomb")
        glBindTexture(GL_TEXTURE_2D, bomb_texture.id);
    else if(type == "missile")
        glBindTexture(GL_TEXTURE_2D, missile_texture.id);
    else if(type == "armour")
        glBindTexture(GL_TEXTURE_2D, armour_texture.id);
    else if(type == "winner1")
       glBindTexture(GL_TEXTURE_2D, winner1_texture.id);
    else if(type == "winner2")
        glBindTexture(GL_TEXTURE_2D, winner2_texture.id);
    else if(type == "key")
        glBindTexture(GL_TEXTURE_2D, key_texture.id);
    else if(type == "pause")
        glBindTexture(GL_TEXTURE_2D, pause_texture.id);
    else if(type == "story")
        glBindTexture(GL_TEXTURE_2D, story_texture.id);
    else if(type == "gameover")
        glBindTexture(GL_TEXTURE_2D, gameover_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Info::scale()
{
    if(type == "tool") {
        m_scale.x = -0.12f * ViewHelper::getRatio();
        m_scale.y = 0.12f * ViewHelper::getRatio();
    }
    else {
        m_scale.x = -0.25f * ViewHelper::getRatio();
        m_scale.y = 0.25f * ViewHelper::getRatio();
    }

}

void Info::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteProgram(effect.program);
}
