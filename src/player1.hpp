#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include <list>
#include <iostream>
#include "freeze.hpp"
#include "water.hpp"
#include "arms.hpp"
#include "antidote.hpp"

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

    void animate();
    
    bool collides_with(const Freeze& freeze);
    bool collides_with(const Water& water);
    bool collides_with(const Arms& arm);
    bool collides_with(const Antidote& antidote);

private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_position; // Window coordinates
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

    std::list<int> m_keys;
};