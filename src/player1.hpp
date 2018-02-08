#pragma once

#include "common.hpp"

class Player1 : public Renderable
{
    static Texture player1_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen);
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void update(float ms);

    void move(vec2 off);

    // Returns the current salmon position
    vec2 get_position()const;
    
    // True if the salmon is alive
    bool is_alive()const;

    void set_key(int direction, bool pressed);

    void start_animate(int direction);

    void animate(int direction);

private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

    bool m_keys[4];
};