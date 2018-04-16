#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "Ice.hpp"
#include "water.hpp"
#include "antidote.hpp"
#include "Limb.h"
#include "bomb.hpp"
#include "armour.hpp"
#include "missile.hpp"
#include "mud.hpp"
#include "punchleft.hpp"
#include "punchright.hpp"

class Player2 : public Renderable
{
    static Texture player2_texture;
public:
    // Creates all the associated render resources and default transform
    bool init(vec2 screen, std::vector<vec2> mapCollisionPoints);
    
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
    inline void increase_speed_legs(float newSpeed) {
        speed += newSpeed;
    }
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
    
    void set_droppedantidotestate(bool delay);
    void set_explosion(bool delay);
    void set_punched(bool waspunched);

    // True if the salmon is alive
    bool is_alive()const;

    void set_key(int key, bool pressed);
    vec2 get_shootDirection();

    void animate();

    void increase_speed();
    void decrease_speed();

    bool collides_with(const Limb& limb);

    bool collides_with(const Ice& freeze);
    bool collides_with(const Water& water);
    bool collides_with(const Antidote& antidote);
    bool collides_with(const Bomb& bomb);
    bool collides_with(const Armour& armour);
    bool collides_with(const Missile& missile);
    bool collides_with(const Mud& mud);
    bool collides_with(const Punchright& punchright);
    bool collides_with(const Punchleft& punchleft);

    vec2 get_bounding_box()const;

    bool isBoundingBoxForFeetInsidePolygon(float, float);

    void destroy();
    
    int numberofHits;
    int lastkey;

private:
    bool m_is_alive; // True if the salmon is alive
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    int m_speed;

    float mass;
    float speed;
    float speedlegs;
    float originalSpeed;
    bool blowback;
    vec2 blowbackForce;
    std::vector<vec2> m_mapCollisionPoints;

    bool m_keys[4];
    vec2 shootdirection;

    bool frozen;
    bool armour_in_use;
    bool droppedantidotedelay;
    bool explosiondelay;
    bool punched;
};
