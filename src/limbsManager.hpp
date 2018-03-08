#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "arms.hpp"
#include "legs.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include <map>

class LimbsManager
{

public:
    // initialize a limbsManager
    bool init(vec2 screen);
    
    // Renders the existing limbs
    void draw(const mat3& projection);

   //spawn new arm in random
    bool spawn_arms();

    //spawn new leg in random
    bool spawn_legs();

    //find most optimal clusters of leg clusters and arm clusters to make a common enemy zombie
    bool cluster_limbs();
   
    //check if players collide with any limbs
    int check_collision_with_players(Player1 *p1, Player2 *p2);

    size_t get_arms_size();

    size_t get_legs_size();

    std::vector<Arms> getArms() {
        return m_arms;
    }

    std::vector<Legs> getLegs() {
        return m_legs;
    }

    std::vector<Limb> getLimbs() {
        return limbs;
    }

    void destroy();
    
private:
    vec2 m_screen;
    std::vector<Arms> m_arms;
    std::vector<Legs> m_legs;
    std::vector<Limb> limbs;
};