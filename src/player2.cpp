// Header
#include "player2.hpp"

#include <cmath>

Texture Player2::player2_texture;
int currFrame_p2 = 0;
auto startTime_p2 = std::chrono::high_resolution_clock::now();
int frameTime_p2 = 100;

bool Player2::init(vec2 screen) {
    // Load shared texture
    if (!player2_texture.is_valid()) {
        if (!player2_texture.load_from_file(p2_textures_path("p2.png"))) {
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
    m_scale.x = -0.10f * ViewHelper::getRatio();;
    m_scale.y = 0.10f * ViewHelper::getRatio();;
    m_is_alive = true;
    m_position = {(screen.x - (screen.x / 5)) * ViewHelper::getRatio(), (screen.y / 2) * ViewHelper::getRatio()};
    return true;
}

void Player2::draw(const mat3 &projection) {
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
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) 0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, player2_texture.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *) &transform);
    float color[] = {1.f, 1.f, 1.f};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *) &projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Player2::set_key(int key, bool pressed) {
    if (pressed)
        m_keys[key] = true;
    if (!pressed)
        m_keys[key] = false;
}

vec2 Player2::get_position() const {
    return m_position;
}

bool Player2::is_alive() const {
    return m_is_alive;
}

void Player2::update(float ms) {
    const float PLAYER_SPEED = 500.f;
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
    if (m_keys[3]) {
        move({step, 0});
        animate(3);
    } else {
        // stop_animate();
    }
}

void Player2::move(vec2 off) {
    m_position.x += off.x;
    m_position.y += off.y;
}

void Player2::animate(int direction) {
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

    if (milliseconds > frameTime_p2) {
        if (m_keys[0]) {
            switch (currFrame_p2) {
                case 0: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {1 / 5.f, 1.f};
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
                case 1: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {2 / 5.f, 1.f};
                    vertices[1].position = {+wr, +hr, -0.02f};
                    vertices[1].texcoord = {1 / 5.f, 1.f};
                    vertices[2].position = {+wr, -hr, -0.02f};
                    vertices[2].texcoord = {1 / 5.f, 2 / 3.f};
                    vertices[3].position = {-wr, -hr, -0.02f};
                    vertices[3].texcoord = {2 / 5.f, 2 / 3.f};

                    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                    currFrame_p2 = 0;
                    startTime_p2 = currTime;
                    break;
                }
                case 2: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {1 / 5.f, 1.f};
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
        } else if (m_keys[1]) {
            switch (currFrame_p2) {
                case 0: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {1 / 5.f, 2 / 3.f};
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
                case 1: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {2 / 5.f, 2 / 3.f};
                    vertices[1].position = {wr, +hr, -0.02f};
                    vertices[1].texcoord = {1 / 5.f, 2 / 3.f};
                    vertices[2].position = {+wr, -hr, -0.02f};
                    vertices[2].texcoord = {1 / 5.f, 1 / 3.f};
                    vertices[3].position = {-wr, -hr, -0.02f};
                    vertices[3].texcoord = {2 / 5.f, 1 / 3.f};

                    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                    currFrame_p2 = 2;
                    startTime_p2 = currTime;
                    break;
                }
                case 2: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {3 / 5.f, 2 / 3.f};
                    vertices[1].position = {wr, +hr, -0.02f};
                    vertices[1].texcoord = {2 / 5.f, 2 / 3.f};
                    vertices[2].position = {+wr, -hr, -0.02f};
                    vertices[2].texcoord = {2 / 5.f, 1 / 3.f};
                    vertices[3].position = {-wr, -hr, -0.02f};
                    vertices[3].texcoord = {3 / 5.f, 1 / 3.f};

                    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                    currFrame_p2 = 0;
                    startTime_p2 = currTime;
                    break;
                }
            }
        } else if (m_keys[2]) {
            switch (currFrame_p2) {
                case 0: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {3 / 5.f, 1 / 3.f};
                    vertices[1].position = {wr, +hr, -0.02f};
                    vertices[1].texcoord = {2 / 5.f, 1 / 3.f};
                    vertices[2].position = {+wr, -hr, -0.02f};
                    vertices[2].texcoord = {2 / 5.f, 0 / 3.f};
                    vertices[3].position = {-wr, -hr, -0.02f};
                    vertices[3].texcoord = {3 / 5.f, 0 / 3.f};

                    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                    currFrame_p2 = 1;
                    startTime_p2 = currTime;
                    break;
                }
                case 1: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {4 / 5.f, 1 / 3.f};
                    vertices[1].position = {wr, +hr, -0.02f};
                    vertices[1].texcoord = {3 / 5.f, 1 / 3.f};
                    vertices[2].position = {+wr, -hr, -0.02f};
                    vertices[2].texcoord = {3 / 5.f, 0 / 3.f};
                    vertices[3].position = {-wr, -hr, -0.02f};
                    vertices[3].texcoord = {4 / 5.f, 0 / 3.f};

                    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                    currFrame_p2 = 0;
                    startTime_p2 = currTime;
                    break;
                }
                case 2: {
                    TexturedVertex vertices[4];

                    vertices[0].position = {-wr, +hr, -0.02f};
                    vertices[0].texcoord = {3 / 5.f, 1 / 3.f};
                    vertices[1].position = {wr, +hr, -0.02f};
                    vertices[1].texcoord = {2 / 5.f, 1 / 3.f};
                    vertices[2].position = {+wr, -hr, -0.02f};
                    vertices[2].texcoord = {2 / 5.f, 0 / 3.f};
                    vertices[3].position = {-wr, -hr, -0.02f};
                    vertices[3].texcoord = {3 / 5.f, 0 / 3.f};

                    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexturedVertex) * 4, vertices);

                    currFrame_p2 = 1;
                    startTime_p2 = currTime;
                    break;
                }
            }
        } else if (m_keys[3]) {
            switch (currFrame_p2) {
                case 0: {
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
                case 1: {
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
                case 2: {
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


void Player2::increase_speed() {
    m_speed = fmin(m_speed + 1, 4);
}

void Player2::decrease_speed() {
    m_speed = fmax(m_speed - 1, 1);
}

// Simple bounding box collision check,
bool Player2::collides_with(const Ice &freeze) {
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

bool Player2::collides_with(const Water &water) {
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

bool Player2::collides_with(const Limb &limb) {
    float dx = m_position.x - limb.get_position().x;
    float dy = m_position.y - limb.get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(limb.get_bounding_box().x, limb.get_bounding_box().y);
    float my_r = std::max(m_scale.x, m_scale.y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    return d_sq < r * r;
}

bool Player2::collides_with(const Antidote &antidote) {
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

void Player2::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void Player2::addPlayerToWorld(b2World *world) {
    addBodyToWorld(world);
    body->SetBullet(true);
}

void Player2::addFixturesToBody() {
    this->addCircularFixtureToBody(0.7f);
}