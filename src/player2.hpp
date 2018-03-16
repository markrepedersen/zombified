#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "arms.hpp"
#include "legs.hpp"
#include "Ice.hpp"
#include "legs.hpp"
#include "water.hpp"
#include "antidote.hpp"
#include "Limb.h"
#include "PhysicalBody.h"
#include <Box2D.h>
#include "bomb.hpp"
#include "armour.hpp"
#include "missile.hpp"

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
    
    float get_mass()const;
    void set_mass(float newMass);
    float get_speed()const;
    void set_speed(float newSpeed);
    float get_speed_legs()const;
    void increase_speed_legs(float newSpeed);
    bool get_blowback()const;
    void set_blowback(bool newblowback);
    vec2 get_blowbackForce()const;
    void set_blowbackForce(vec2 newblowbackForce);

    // True if the salmon is alive
    bool is_alive()const;

    void set_key(int key, bool pressed);

    void animate(int direction);

    void increase_speed();
    void decrease_speed();

    bool collides_with(const Ice& freeze);
    bool collides_with(const Water& water);
    bool collides_with(const Arms& arm);
    bool collides_with(const Legs& legs);
    bool collides_with(const Antidote& antidote);
    bool collides_with(const Bomb& bomb);
    bool collides_with(const Armour& armour);
    bool collides_with(const Missile& missile);

    void destroy();

private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    int m_speed;
    vec2 m_position;
    bool m_keys[4];

    float mass;
    float speed;
    float speedlegs;
    bool blowback;
    vec2 blowbackForce;

    bool m_keys[4];
};