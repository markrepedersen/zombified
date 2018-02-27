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
    bool spawn_arms(); // arm init, keep track of list, set position as random
    // arm.setCurrentTarget({0,0});
        // arm.setLastTarget(arm.getCurrentTarget());
        // m_arms.emplace_back(arm);
        // return true;

    //spawn new leg in random
    bool spawn_legs();

    //everytime a limb is spawned or a limb collided with a player, we need to update the clusters
    void update_clusters();

    //find most optimal clusters of legs and updates the mapping
    void pair_legs();

    //find most optimal clusters of arms and updates the mapping
    void pair_arms();

    //find most optimal clusters of leg clusters and arm clusters to make a common enemy zombie
    void cluster_limbs();

    double distance_between_pairs(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a);

    vec2 get_centroid(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a);
    
    //update the target locations of each limb
    void update_limb_targets();
   
    //check if players collide with any limbs
    int check_collision_with_players(Player1 *p1, Player2 *p2);

   //check if any limbs collided, and if yes, combine them into one
   void checkLimbCollisions();
   //if collide, delete rest keep 1, update 1's value respectively

    size_t get_arms_size();

    size_t get_legs_size();

    void destroy();
    
    
private:
    vec2 m_screen;
    std::vector<Arms> m_arms;
    std::vector<Legs> m_legs;

    std::map<Legs*, Legs*> m_leg_pairs;
    std::map<Arms*, Arms*> m_arm_pairs;
    //maybe
    // std::vector<vec2> m_armCentroids;
    // std::vector<vec2> m_legCentroids;
    std::vector<vec2> m_limbCentroids;
    //
    std::map<vec2, std::pair<Legs*, Legs*> > clusters;



    
};