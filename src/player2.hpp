#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "arms.hpp"
#include "freeze.hpp"
#include "water.hpp"
#include "antidote.hpp"

class Player2 : public Renderable
{
    static Texture player2_texture;
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

    void set_key(int key, bool pressed);

    void start_animate(int direction);

    void animate(int direction);
    
    bool collides_with(const Freeze& freeze);
    bool collides_with(const Water& water);
    bool collides_with(const Arms& arm);
    bool collides_with(const Antidote& antidote);
    
    void destroy();
private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

    bool m_keys[4]; 
};