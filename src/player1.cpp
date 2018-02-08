// Header
#include "player1.hpp"

#include <cmath>

Texture Player1::player1_texture;
int currFrame = 0;
auto startTime = std::chrono::high_resolution_clock::now();
int frameTime = 100;

bool Player1::init(vec2 screen)
{
    // Load shared texture
    if (!player1_texture.is_valid())
    {
        if (!player1_texture.load_from_file(p1_textures_path("p1.png")))
        {
            fprintf(stderr, "Failed to load player1 texture!");
            return false;
        }
    }

    int frameIndex = 1;
    int spriteWidth = 225;
    int spriteHeight = 365;

    int numPerRow = player1_texture.width / spriteWidth;
    int numPerCol = player1_texture.height / spriteHeight;

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
    uint16_t indices[] = {0, 3, 1, 1, 3, 2};

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
    m_scale.x = -0.25f;
    m_scale.y = 0.25f;
    m_is_alive = true;
    m_position = {screen.x - 1150.f, screen.y - 450.f};

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

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Player1::set_key(int key, bool pressed)
{
    if (pressed) {
        // Reset all keys to allow for only uni-directional movement
        for (int i = 0; i < 4; i++) {
            m_keys[i] = false;
        }
        m_keys[key] = true;
    }
    if (!pressed)
        m_keys[key] = false;
}

vec2 Player1::get_position() const
{
    return m_position;
}

bool Player1::is_alive() const
{
    return m_is_alive;
}

void Player1::update(float ms)
{
    const float PLAYER_SPEED = 200.f;
    float step = PLAYER_SPEED * (ms / 1000);

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
    else
    {
        // stop_animate();
    }
}

void Player1::move(vec2 off)
{
    m_position.x += off.x;
    m_position.y += off.y;
}

void Player1::start_animate(int direction)
{
    switch (direction)
    {
    case 3:
        animate(3);
    }
}

void Player1::animate(int direction)
{
    int frameIndex = 1;
    int spriteWidth = 225;
    int spriteHeight = 365;

    int numPerRow = player1_texture.width / spriteWidth;
    int numPerCol = player1_texture.height / spriteHeight;

    // The position corresponds to the center of the texture
    float wr = spriteWidth;
    float hr = spriteHeight;

    auto currTime = std::chrono::high_resolution_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currTime-startTime).count();

    if (milliseconds > frameTime) {
    if (m_keys[3] && currFrame == 0)
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

        currFrame = 1;
        startTime = currTime;
    }
    else if (m_keys[3] && currFrame == 1) {
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

        currFrame = 2;
        startTime = currTime;
    }
    else if (m_keys[3] && currFrame == 2) {
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

        currFrame = 0;
        startTime = currTime;
    }
    }
}