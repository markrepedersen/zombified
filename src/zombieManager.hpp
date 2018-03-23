#pragma once

#include "common.hpp"
#include "viewHelper.hpp"
#include "zombie.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include "toolboxManager.hpp"
#include "MapGrid.h"
#include "JPS.h"
#include <map>

class ZombieManager
{

public:
    // initialize a zombie manager
    bool init(vec2 screen, const std::vector<vec2> &mapCollisionPoints);
    
    // Renders the existing zombies
    void draw(const mat3& projection);
 
   //spawn new zombie where clusters meet
    bool spawn_zombie(vec2 zombie_pos, vec2 player1_pos, vec2 player2_pos);

    void check_targets();

    //check if players collide with any zombies
    //make sure to apply timeout
    int check_collision_with_players(Player1 *p1, Player2 *p2, ToolboxManager *tb);

    //decide which player for zombie to target
    void computeZPaths(float ms, const MapGrid &mapGrid);

    void destroy();
    
private:
    vec2 m_screen;
    std::vector<Zombie> zombies;
    std::vector<vec2> m_mapCollisionPoints;
    
};