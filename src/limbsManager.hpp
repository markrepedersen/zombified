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
    static LimbsManager* getInstance(vec2 screen);
    // initialize a limbsManager
    // bool init(vec2 screen);
    
    // Renders the existing limbs
    static void draw(const mat3& projection);

   //spawn new arm in random
    static bool spawn_arms();

    //spawn new leg in random
    static bool spawn_legs();

    //find most optimal clusters of leg clusters and arm clusters to make a common enemy zombie
    static bool cluster_limbs();
   
    //check if players collide with any limbs
    static int check_collision_with_players(Player1 *p1, Player2 *p2);

    static size_t get_arms_size();

    static size_t get_legs_size();

    static std::vector<Arms>* getArms();

    static std::vector<Legs>* getLegs();

    static std::vector<Limb>* getLimbs();
    // void destroy();
    
private:
    LimbsManager();
    static LimbsManager* limbsManagerInstance;
    vec2 m_screen;
    std::vector<Arms> m_arms;
    std::vector<Legs> m_legs;
    std::vector<Limb> limbs;
};