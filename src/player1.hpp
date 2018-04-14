#pragma once

#include "common.hpp"
#include "Ice.hpp"
#include "Limb.h"
#include "water.hpp"
#include "antidote.hpp"
#include "bomb.hpp"
#include "armour.hpp"
#include "missile.hpp"
#include "mud.hpp"
#include "player2.hpp"
#include "punchright.hpp"
#include "punchleft.hpp"
#include "blood.hpp"
#include "Kinetic.h"

class Player1 : public Kinetic
{
    static Texture player1_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen, std::vector<vec2> mapCollisionPoints);
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void update(float ms);

    // GETTERS AND SETTERS
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
    
    float get_originalspeed()const;
    void set_originalspeed(float newSpeed);

    void set_freezestate(bool newFreezeState);
    bool get_freezestate()const;

    void set_armourstate(bool newArmourState);
    bool get_armourstate()const;

    // True if the salmon is alive
    bool is_alive()const;

    void set_key(int direction, bool pressed);
    vec2 get_shootDirection();

    void animate();
    void create_blood(vec2 position);

    bool collides_with(const Ice& freeze);
    bool collides_with(Limb& limb);
    bool collides_with(const Water& water);
    bool collides_with(const Antidote& antidote);
    bool collides_with(const Bomb& bomb);
    bool collides_with(const Armour& armour);
    bool collides_with(const Missile& missile);
    bool collides_with(const Mud& mud);
    bool collides_with(const Player2& player2);
    bool collides_with(const Punchright& punchright);
    bool collides_with(const Punchleft& punchleft);

    vec2 get_bounding_box() const;
    void destroy();

    //float negy;
    //float negx;
    int numberofHits;
    int lastkey;

    void on_player1_collision(Kinetic *player)override;
    void on_player2_collision(Kinetic *player)override;
    void on_antidote_collision(Kinetic *antidote)override;
    void on_limb_collision(Kinetic *limb)override;
    void on_armour_collision(Kinetic *player)override;
    void on_explosion_collision(Kinetic *explosion)override;
    void on_ice_collision(Kinetic *ice)override;
    void on_missile_collision(Kinetic *missile)override;
    void on_water_collision(Kinetic *water)override;
    void on_zombie_collision(Kinetic *zombie)override;

    vec2 getAABB()override;

private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    int m_speed;

    float mass;
    float speed;
    float originalSpeed;
    bool blowback;
    vec2 blowbackForce;
    float speedlegs;
    std::vector<vec2> m_mapCollisionPoints;

    bool m_keys[4];
    vec2 shootdirection;

    Blood m_blood;
    bool frozen;
    bool armour_in_use;
    bool affectedByMud;
};
