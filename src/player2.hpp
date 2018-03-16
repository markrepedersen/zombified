#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "arms.hpp"
#include "legs.hpp"
#include "Ice.hpp"
#include "water.hpp"
#include "antidote.hpp"
#include "Limb.h"
#include "PhysicalBody.h"
#include <Box2D.h>

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

    void animate(int direction);

    void increase_speed();
    void decrease_speed();
    
    bool collides_with(const Ice& freeze);
    bool collides_with(const Water& water);
    // bool collides_with(const Arms& arm);
    // bool collides_with(const Legs& leg);
    bool collides_with(const Limb& limb);
    bool collides_with(const Antidote& antidote);
    
    void destroy();

private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    int m_speed;
    vec2 m_position;
    bool m_keys[4];
};