#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include "toolboxManager.hpp"
#include "MapGrid.h"
#include "JPS.h"
#include <map>

class LimbsManager
{

public:
    // initialize a limbsManager
    bool init(vec2 screen, const std::vector<vec2> &mapCollisionPoints);
    
    // Renders the existing limbs
    void draw(const mat3& projection);

   //spawn new arm in random
    bool spawn_arms();

    //spawn new leg in random
    bool spawn_legs();

    //find most optimal clusters of leg clusters and arm clusters to make a common enemy zombie
    bool cluster_limbs();
   
    //check if players collide with any limbs
    int check_collision_with_players(Player1 *p1, Player2 *p2, ToolboxManager *tb);

    // size_t get_arms_size();
    int get_arms_size();

    // size_t get_legs_size();
    int get_legs_size();

    std::vector<Limb> getLimbs() {
        return limbs;
    }

    void computePaths(float ms, const MapGrid &mapGrid);

    void destroy();

    vec2 getRandomPointInMap();
    
private:
    vec2 m_screen;
    std::vector<Limb> limbs;
    int m_arms_total;
    int m_legs_total;
    std::vector<vec2> randomPoints;
};