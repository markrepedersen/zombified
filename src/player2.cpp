// Header
#include "player2.hpp"

#include <cmath>

Texture Player2::player2_texture;
int currFrame_p2 = 1;
auto startTime_p2 = std::chrono::high_resolution_clock::now();
int frameTime_p2 = 100;
const float PLAYER_SPEED = 200.f;

bool Player2::init(vec2 screen)
{
    // Load shared texture
    if (!player2_texture.is_valid())
    {
        if (!player2_texture.load_from_file(p2_textures_path("p2.png")))
        {
            //fprintf(stderr, "Failed to load player2 texture!");
            return false;
        }
    }

    // int frameIndex = 1;
    int spriteWidth = 225;
    int spriteHeight = 365;

    // int numPerRow = player2_texture.width / spriteWidth;
    // int numPerCol = player2_texture.height / spriteHeight;
    
    // The position corresponds to the center of the texture
    float wr = spriteWidth;  //* 0.5f;
    float hr = spriteHeight; //* 0.5f;
    
    TexturedVertex vertices[4];
    vertices[0].position = {-wr, +hr, -0.02f};
    vertices[0].texcoord = {-1 / 5.f, 1 / 3.f};
    vertices[1].position = {+wr, +hr, -0.02f};
    vertices[1].texcoord = {-0.f, 1 / 3.f};
    vertices[2].position = {+wr, -hr, -0.02f};
    vertices[2].texcoord = {-0.f, 0.f};
    vertices[3].position = {-wr, -hr, -0.02f};
    vertices[3].texcoord = {-1 / 5.f, 0.f};
    
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
    
    
    speed = PLAYER_SPEED;
    speedlegs = PLAYER_SPEED;
    mass = 1.0;
    blowback = false;
    
    // Setting initial values
    m_scale.x = -0.10f * ViewHelper::getRatio();;
    m_scale.y = 0.10f * ViewHelper::getRatio();;
    m_is_alive = true;
    m_position = { (screen.x-(screen.x/5)) * ViewHelper::getRatio(), (screen.y/2) * ViewHelper::getRatio()};
    
    return true;
}

void Player2::draw(const mat3& projection)
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
    glBindTexture(GL_TEXTURE_2D, player2_texture.id);
    
    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    
    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Player2::set_key(int key, bool pressed) {
    if (pressed)
        m_keys[key] = true;
    if (!pressed)
        m_keys[key] = false;
}

vec2 Player2::get_position()const
{
    return m_position;
}

float Player2::get_mass() const
{
    return mass;
}

void Player2::set_mass(float newMass)
{
    mass = newMass;
}

float Player2::get_speed() const
{
    return speed;
}
void Player2::set_speed(float newSpeed)
{
    speed = newSpeed;
}

float Player2::get_speed_legs()const
{
    return speedlegs;
}
void Player2::increase_speed_legs(float newSpeed)
{
    speedlegs = newSpeed;
    set_speed(speedlegs);
}

bool Player2::get_blowback()const
{
    return blowback;
}
void Player2::set_blowback(bool newblowback)
{
    blowback = newblowback;
}
vec2 Player2::get_blowbackForce()const
{
    return blowbackForce;
}
void Player2::set_blowbackForce(vec2 newblowbackForce)
{
    blowbackForce = newblowbackForce;
}

bool Player2::is_alive()const
{
    return m_is_alive;
}

void Player2::update(float ms)
{
    //const float PLAYER_SPEED = 200.f;
    float step = step = speed * (ms / 1000);

    if (m_keys[0])
        move({0, -step});
        animate(0);
    if (m_keys[1])
        move({-step, 0});
        animate(2);
    if (m_keys[2])
        move({0, step});
        animate(2);
    if (m_keys[3])
    {
        move({step, 0});
        animate(3);
    }
    if (blowback)
    {
        float x = get_blowbackForce().x*(ms/1000)*(get_speed()/100);
        float y = get_blowbackForce().y*(ms/1000)*(get_speed()/100);
        move({x, y});
    }
    
    else
    {
        // stop_animate();
    }
}

void Player2::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Player2::animate(int direction)
{
    // int frameIndex = 1;
    int spriteWidth = 225;
    int spriteHeight = 365;

    // int numPerRow = player2_texture.width / spriteWidth;
    // int numPerCol = player2_texture.height / spriteHeight;

    // The position corresponds to the center of the texture
    float wr = spriteWidth;
    float hr = spriteHeight;

    auto currTime = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - startTime_p2).count();

    if (milliseconds > frameTime_p2)
    {
        if (m_keys[0])
        {
            switch (currFrame_p2)
            {
            case 0:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {1/5.f, 1.f};
                vertices[1].position = {+wr, +hr, -0.02f};
                vertices[1].texcoord = {0.f, 1.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {0.f, 2 / 3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {1 / 5.f, 2 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 1;
                startTime_p2 = currTime;
                break;
            }
            case 1:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {2/5.f, 1.f};
                vertices[1].position = {+wr, +hr, -0.02f};
                vertices[1].texcoord = {1/5.f, 1.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {1/5.f, 2/3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {2/ 5.f, 2 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 0;
                startTime_p2 = currTime;
                break;
            }
            case 2:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {1/5.f, 1.f};
                vertices[1].position = {+wr, +hr, -0.02f};
                vertices[1].texcoord = {0.f, 1.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {0.f, 2 / 3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {1 / 5.f, 2 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 1;
                startTime_p2 = currTime;
                break;
            }
            }
        }
        else if (m_keys[1])
        {
            switch (currFrame_p2)
            {
            case 0:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {1/5.f, 2 / 3.f};
                vertices[1].position = {+wr, +hr, -0.02f};
                vertices[1].texcoord = {0.f, 2 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {0.f, 1 / 3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {1 / 5.f, 1 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 1;
                startTime_p2 = currTime;
                break;
            }
            case 1:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {2/5.f, 2 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {1/5.f, 2 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {1/5.f, 1 / 3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {2/5.f, 1 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 2;
                startTime_p2 = currTime;
                break;
            }
            case 2:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {3 / 5.f, 2 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {2 / 5.f, 2 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {2 / 5.f, 1/3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {3 / 5.f, 1/3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 0;
                startTime_p2 = currTime;
                break;
            }
            }
        }
        else if (m_keys[2])
        {
            switch (currFrame_p2)
            {
            case 0:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {3 / 5.f, 1 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {2 / 5.f, 1 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {2 / 5.f, 0/3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {3 / 5.f, 0/3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 1;
                startTime_p2 = currTime;
                break;
            }
            case 1:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {4 / 5.f, 1 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {3 / 5.f, 1 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {3 / 5.f, 0/3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {4 / 5.f, 0/3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 0;
                startTime_p2 = currTime;
                break;
            }
            case 2:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {3 / 5.f, 1 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {2 / 5.f, 1 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {2 / 5.f, 0/3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {3 / 5.f, 0/3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 1;
                startTime_p2 = currTime;
                break;
            }
            }
        }
        else if (m_keys[3])
        {
            switch (currFrame_p2)
            {
            case 0:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {4 / 5.f, 2 / 3.f};
                vertices[1].position = {+wr, +hr, -0.02f};
                vertices[1].texcoord = {3 / 5.f, 2 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {3 / 5.f, 1 / 3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {4 / 5.f, 1 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 1;
                startTime_p2 = currTime;
                break;
            }
            case 1:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {5 / 5.f, 2 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {4 / 5.f, 2 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {4 / 5.f, 1 / 3.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {5 / 5.f, 1 / 3.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 2;
                startTime_p2 = currTime;
                break;
            }
            case 2:
            {
                TexturedVertex vertices[4];

                vertices[0].position = {-wr, +hr, -0.02f};
                vertices[0].texcoord = {5 / 5.f, 1 / 3.f};
                vertices[1].position = {wr, +hr, -0.02f};
                vertices[1].texcoord = {4 / 5.f, 1 / 3.f};
                vertices[2].position = {+wr, -hr, -0.02f};
                vertices[2].texcoord = {4 / 5.f, 0.f};
                vertices[3].position = {-wr, -hr, -0.02f};
                vertices[3].texcoord = {5 / 5.f, 0.f};

                glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                currFrame_p2 = 0;
                startTime_p2 = currTime;
                break;
            }
            }
        }
    }
}

// Simple bounding box collision check,
bool Player2::collides_with(const Freeze& freeze)
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

bool Player2::collides_with(const Water& water)
{
    float dx = m_position.x - water.get_position().x;
    float dy = m_position.y - water.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(water.get_bounding_box().x, water.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool Player2::collides_with(const Arms& arm)
{
    float dx = m_position.x - arm.get_position().x;
    float dy = m_position.y - arm.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(arm.get_bounding_box().x, arm.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool Player2::collides_with(const Antidote& antidote)
{
    float dx = m_position.x - antidote.get_position().x;
    float dy = m_position.y - antidote.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(antidote.get_bounding_box().x, antidote.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool Player2::collides_with(const Legs& leg)
{
    float dx = m_position.x - leg.get_position().x;
    float dy = m_position.y - leg.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(leg.get_bounding_box().x, leg.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

void Player2::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);
    
    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}