// Header
#include "player2.hpp"

#include <cmath>
#include <iostream>

Texture Player2::player2_texture;

// current frame in animation
int curr_frame_p2 = 0;
// frame to draw and previous frame in sprite
int sprite_frame_index_p2 = 0;
// sprite information
int sprite_width_p2 = 225;
int sprite_height_p2 = 361;
int num_rows_p2 = 3;
int num_cols_p2 = 5;
int up_frames_p2 [2] = {10, 11};
int left_frames_p2 [3] = {5, 6, 7};
int down_frames_p2 [2] = {2, 3};
int right_frames_p2 [3] = {8, 9, 4};
// animation timing
int frame_time_p2 = 100;
auto start_time_p2 = std::chrono::high_resolution_clock::now();

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

    // The position corresponds to the center of the texture
    float wr = sprite_width_p2 * 0.5f;
    float hr = sprite_height_p2 * 0.5f;

    TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 1/5.f, 1/3.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 0.f, 1/3.f };
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
    if (!effect.load_from_file(shader_path("player.vs.glsl"), shader_path("player.fs.glsl")))
        return false;
    
    
    speed = PLAYER_SPEED;
    speedlegs = PLAYER_SPEED;
    mass = 1.0;
    blowback = false;
    
    // Setting initial values
    m_scale.x = -0.2f * ViewHelper::getRatio();
    m_scale.y = 0.2f * ViewHelper::getRatio();
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, player2_texture.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
    float color[] = {1.f, 1.f, 1.f};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

    // color changing
    float timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    glUniform3f(vertexColorLocation, 0.0f, greenValue, 0.0f);
    
    // Specify uniform variables
    glUniform1iv(sprite_frame_index_uloc, 1, &sprite_frame_index_p2);
    glUniform1iv(num_rows_uloc, 1, &num_rows_p2);
    glUniform1iv(num_cols_uloc, 1, &num_cols_p2);

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
    auto curr_time = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time_p2).count();

    if (milliseconds > frame_time_p2)
    {
        if (m_keys[0])
        {
            curr_frame_p2 = (curr_frame_p2 + 1) % 2;
            sprite_frame_index_p2 = up_frames_p2[curr_frame_p2];

            start_time_p2 = curr_time;

            // Debugging
	        // int row = sprite_frame_index_p2 / 5;
	        // int col = sprite_frame_index_p2 - row * 5;
	        // float tex_x = col / 5.0;
	        // float tex_y = row / 3.0;
            // std::cout << "Index: " << sprite_frame_index_p2 << ", diff_tex_x: " << tex_x << ", diff_tex_y: " << tex_y << "\n";
        }
        else if (m_keys[1])
        {
            curr_frame_p2 = (curr_frame_p2 + 1) % 3;
            sprite_frame_index_p2 = left_frames_p2[curr_frame_p2];

            start_time_p2 = curr_time;
        }
        else if (m_keys[2])
        {
            curr_frame_p2 = (curr_frame_p2 + 1) % 2;
            sprite_frame_index_p2 = down_frames_p2[curr_frame_p2];

            start_time_p2 = curr_time;
        }
        else if (m_keys[3])
        {
            curr_frame_p2 = (curr_frame_p2 + 1) % 3;
            sprite_frame_index_p2 = right_frames_p2[curr_frame_p2];

            start_time_p2 = curr_time;
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