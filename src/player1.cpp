// Header
#include "player1.hpp"

#include <cmath>
#include <iostream>

Texture Player1::player1_texture;
// current frame in animation
int curr_frame_p1 = 0;
// frame to draw and previous frame in sprite
int sprite_frame_index_p1 = 0;
// sprite information
int sprite_width_p1 = 225;
int sprite_height_p1 = 365;
int num_rows_p1 = 3;
int num_cols_p1 = 5;
int up_frames_p1 [2] = {10, 11};
int left_frames_p1 [3] = {5, 6, 7};
int down_frames_p1 [2] = {2, 3};
int right_frames_p1 [3] = {8, 9, 4};
// animation timing
int frame_time_p1 = 100;
auto start_time_p1 = std::chrono::high_resolution_clock::now();

const float PLAYER_SPEED = 200.f;

bool Player1::init(vec2 screen, std::vector<vec2> mapCollisionPoints)
{
    //set mapCollisionPoints as the same on with world
    m_mapCollisionPoints = mapCollisionPoints;
    // Load shared texture
    if (!player1_texture.is_valid()) {
        if (!player1_texture.load_from_file(p1_textures_path("p1.png"))) {
            //fprintf(stderr, "Failed to load player1 texture!");
            return false;
        }
    }

    // The position corresponds to the center of the texture
    float wr = sprite_width_p1 * 0.5f;
    float hr = sprite_height_p1 * 0.5f;

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

    // Setting initial values
    m_scale.x = -0.2f * ViewHelper::getRatio();
    m_scale.y = 0.2f * ViewHelper::getRatio();
    m_is_alive = true;

    // m_position = {screen.x - 1150.f, screen.y - 450.f};
    speed = PLAYER_SPEED;
    speedlegs = PLAYER_SPEED;
    mass = 1.0;
    blowback = false;

    m_position = {(screen.x * ViewHelper::getRatio()) / 5, (screen.y * ViewHelper::getRatio()) / 2};

    return true;
}

void Player1::draw(const mat3 &projection)
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
    glBindTexture(GL_TEXTURE_2D, player1_texture.id);

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
    glUniform1iv(sprite_frame_index_uloc, 1, &sprite_frame_index_p1);
    glUniform1iv(num_rows_uloc, 1, &num_rows_p1);
    glUniform1iv(num_cols_uloc, 1, &num_cols_p1);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Player1::set_key(int key, bool pressed) {
    if (pressed) {
        m_keys.push_front(key);

        // for (auto v : m_keys) {
        //     std::cout << v << " ";
        // }

        // std::cout << "\n";
    }
    if (!pressed)
        m_keys.remove(key);
        
        // for (auto v : m_keys) {
        //     std::cout << v << " ";
        // }
        
        // std::cout << "\n";
}

vec2 Player1::get_position() const
{
    return m_position;
}

float Player1::get_mass() const
{
    return mass;
}

void Player1::set_mass(float newMass)
{
    mass = newMass;
}

float Player1::get_speed() const
{
    return speed;
}
void Player1::set_speed(float newSpeed)
{
    speed = newSpeed;
}
bool Player1::get_blowback()const
{
    return blowback;
}
void Player1::set_blowback(bool newblowback)
{
    blowback = newblowback;
}
vec2 Player1::get_blowbackForce()const
{
    return blowbackForce;
}
void Player1::set_blowbackForce(vec2 newblowbackForce)
{
    blowbackForce = newblowbackForce;
}

float Player1::get_speed_legs()const
{
    return speedlegs;
}
void Player1::increase_speed_legs(float newSpeed)
{
    speedlegs = newSpeed;
    set_speed(speedlegs);
}

void Player1::update(float ms) {
    float step = speed * (ms / 1000);//PLAYER_SPEED * (ms / 1000);

    if (m_keys.front() == GLFW_KEY_UP)
        {
            if (isInsidePolygon(m_mapCollisionPoints, {m_position.x+0, m_position.y-step}))
            {
                move({0, -step});

                animate();
            }

        }
    if (m_keys.front() == GLFW_KEY_LEFT)
        {
            if (isInsidePolygon(m_mapCollisionPoints, {m_position.x-step, m_position.y + 0}))
            {
                move({-step, 0});
                animate();
            }

        }
    if (m_keys.front() == GLFW_KEY_DOWN)
        {
            if (isInsidePolygon(m_mapCollisionPoints, {m_position.x + 0, m_position.y + step}))
            {
                move({0, step});
                animate();
            }

        }
    if (m_keys.front() == GLFW_KEY_RIGHT)
    {
            if (isInsidePolygon(m_mapCollisionPoints, {m_position.x + step, m_position.y + 0}))
            {
                move({step, 0});

            animate();
            }

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

void Player1::move(vec2 off) {
    m_position.x += off.x;
    m_position.y += off.y;
}

void Player1::animate()
{
    auto curr_time = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time_p1).count();

    if (milliseconds > frame_time_p1)
    {
        if (m_keys.front() == GLFW_KEY_UP)
        {
            curr_frame_p1 = (curr_frame_p1 + 1) % 2;
            sprite_frame_index_p1 = up_frames_p1[curr_frame_p1];
            start_time_p1 = curr_time;
        }
        else if (m_keys.front() == GLFW_KEY_LEFT)
        {
            curr_frame_p1 = (curr_frame_p1 + 1) % 3;
            sprite_frame_index_p1 = left_frames_p1[curr_frame_p1];

            start_time_p1 = curr_time;
        }
        else if (m_keys.front() == GLFW_KEY_DOWN)
        {
            curr_frame_p1 = (curr_frame_p1 + 1) % 2;
            sprite_frame_index_p1 = down_frames_p1[curr_frame_p1];

            start_time_p1 = curr_time;
        }
        else if (m_keys.front() == GLFW_KEY_RIGHT)
        {
            curr_frame_p1 = (curr_frame_p1 + 1) % 3;
            sprite_frame_index_p1 = right_frames_p1[curr_frame_p1];

            start_time_p1 = curr_time;
        }
    }
}

// Simple bounding box collision check,
bool Player1::collides_with(const Ice &freeze) {
    float dx = m_position.x - freeze.get_position().x;
    float dy = m_position.y - freeze.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(freeze.get_bounding_box().x, freeze.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    return d_sq < r * r;

}

bool Player1::collides_with(const Water &water) {
    float dx = m_position.x - water.get_position().x;
    float dy = m_position.y - water.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(water.get_bounding_box().x, water.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    return d_sq < r * r;

}

bool Player1::collides_with(const Limb& limb)
{
    float dx = m_position.x - limb.get_position().x;
    float dy = m_position.y - limb.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(limb.get_bounding_box().x, limb.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    return d_sq < r * r;
}

bool Player1::collides_with(const Antidote &antidote) {
    float dx = m_position.x - antidote.get_position().x;
    float dy = m_position.y - antidote.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(antidote.get_bounding_box().x, antidote.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    return d_sq < r * r;

}


bool Player1::collides_with(const Bomb& bomb)
{
    float dx = m_position.x - bomb.get_position().x;
    float dy = m_position.y - bomb.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(bomb.get_bounding_box().x, bomb.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool Player1::collides_with(const Armour& armour)
{
    float dx = m_position.x - armour.get_position().x;
    float dy = m_position.y - armour.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(armour.get_bounding_box().x, armour.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}
bool Player1::collides_with(const Missile& missile)
{
    float dx = m_position.x - missile.get_position().x;
    float dy = m_position.y - missile.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(missile.get_bounding_box().x, missile.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

void Player1::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}