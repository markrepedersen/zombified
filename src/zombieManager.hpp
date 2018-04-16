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
#include <algorithm>

class ZombieManager
{

public:

    bool init(vec2 screen, const std::vector<vec2> &mapCollisionPoints);

    static ZombieManager* GetInstance() {
        if (instance == nullptr) {
            instance = new ZombieManager();
        }
        return instance;
    }

    bool isColliding(std::vector<vec2>);

    // Renders the existing zombies
    void draw(const mat3& projection);
 
   //spawn new zombie where clusters meet
    bool spawn_zombie(vec2 zombie_pos, vec2 player1_pos, vec2 player2_pos);

    unsigned long getZombieCount();

    void transformZombies(std::vector<Renderable*> &container);

    vec2 update_zombies(float ms, vec2 player1_pos, vec2 player2_pos);

    //check if players collide with any zombies
    //make sure to apply timeout
    int check_collision_with_players(Player1 *p1, Player2 *p2, ToolboxManager *tb);

    //decide which player for zombie to target
    void computeZPaths(float ms, const MapGrid &mapGrid);

    //when the player is attacking using hand combat it will attack at most one zombie in the vicinity
    //per punch
    bool attack_zombies(vec2 player_pos, vec2 bounding_box, int playerNum, ToolboxManager *m_toolboxmanager);

    void setSpeed(float speed);
    
    float getSpeed();
    
    void setZombiesInUse(bool inUse);
    
    bool getZombiesInUse();

    void destroy();
    
private:
    // initialize a zombie manager
    vec2 m_screen;
    std::vector<Zombie> zombies;
    std::vector<vec2> m_mapCollisionPoints;
    float speed;
    bool zombiesInUse;
    static ZombieManager* instance;
};
