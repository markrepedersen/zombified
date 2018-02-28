// Header
#include "limbsManager.hpp"

#include <cmath>

    // initialize a limbsManager
    bool LimbsManager::init(vec2 screen) {
        m_screen = screen;
        return true;
    }
    
    // Renders the existing limbs
    void LimbsManager::draw(const mat3& projection_2D)
    {
        for (auto &arms : m_arms)
            arms.draw(projection_2D);
        for (auto &legs : m_legs)
            legs.draw(projection_2D);
    }

   //spawn new arm in random
    bool LimbsManager::spawn_arms()
    {
        Arms arm;
        if (arm.init())
        {
            arm.setCurrentTarget({0,0});
            arm.setLastTarget(arm.getCurrentTarget());

             m_arms.emplace_back(arm);
            // Setting random initial position
            arm.set_position({(float)((rand() % (int)m_screen.x)),
                (float)((rand() % (int)m_screen.y))});

           
            update_clusters();
            return true;
        }
        fprintf(stderr, "Failed to spawn arm");
        return false;
    } // arm init, keep track of list, set position as random
    // arm.setCurrentTarget({0,0});
    // arm.setLastTarget(arm.getCurrentTarget());
    // m_arms.emplace_back(arm);
    // return true;

    //spawn new leg in random
    bool LimbsManager::spawn_legs()
    {
    Legs leg;
    if (leg.init())
    {
        leg.setCurrentTarget({0,0});
        leg.setLastTarget(leg.getCurrentTarget());
        
        // Setting random initial position
        leg.set_position({(float)((rand() % (int)m_screen.x)),
            (float)((rand() % (int)m_screen.y))});

        m_legs.emplace_back(leg);

        update_clusters();
        return true;
    }
    fprintf(stderr, "Failed to spawn leg");
    return false;
    }

    //everytime a limb is spawned or a limb collided with a player, we need to update the clusters
    void LimbsManager::update_clusters()
    {

        cluster_limbs();
        // update_limb_targets();

    }

    //find most optimal clusters of legs and updates the mapping using k-means
    void LimbsManager::pair_legs()
    {
    }
    //check if leg centroids are empty
    //if empty, find random points
    //if not empty, use points to find new clustering of legs

    //find most optimal clusters of arms and updates the mapping using k-means
    void LimbsManager::pair_arms()
    {
    }
      //check if leg centroids are empty
    //if empty, find random points
    //if not empty, use points to find new clustering of legs

    //find most optimal clusters of leg clusters and arm clusters to make a common enemy zombie using k-means
    void LimbsManager::cluster_limbs()
    {
        
        //vector of all nodes
        //temp list of nodes pointers

       //initialize nodes from limbs with cluster -1
       
       //for each node in vector, find closest other node from list of temp nodes
       //and put them in one cluster
        //check if the other node has a positive cluster key,
            //if yes, add to that cluster on cluster map
                //if this makes 4, remove all those node pointers pointing to nodes from temp lsit
            //if no, create new cluster and add to cluster map
        
        //iterate through all clusters, find its centroids, update targets of each limb

    }
    //check if limbs centroids are empty
    //if empty, find random points
    //if not empty, use points to find new clustering of limbs
    //once new centroids are found, update the targetNode of each limb
    //if new centroid is further from limb than the current targetNode, abort. set the far limb to also go to the closest centroid.



    double LimbsManager::distance_between_pairs(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a)
    {

            //check for null pointers
            return 0.0;

    }

    vec2 LimbsManager::get_centroid(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a)
    {
        std::list<vec2> positions;
        if (l.first != 0)
        {
            positions.push_back(l.first->get_position());
        }

        if (l.second != 0)
        {
            positions.push_back(l.second->get_position());
        }

        if (a.first != 0)
        {
            positions.push_back(a.first->get_position());
        }

        if (a.second != 0)
        {
            positions.push_back(a.second->get_position());
        }

        float max_x;
        float max_y;
        float min_x;
        float min_y;

        for (vec2 position : positions)
        {
            if (max_x == 0)
            {
                max_x = position.x;
            } else if (max_x < position.x) {
                max_x = position.x;
            }

            if (max_y == 0)
            {
                max_y = position.y;
            } else if (max_y < position.y) {
                max_y = position.y;
            }

            if (min_x == 0)
            {
                min_x = position.x;
            } else if (min_x > position.x) {
                min_x = position.x;
            }

            if (min_y == 0)
            {
                min_y = position.y;
            } else if (min_y > position.y) {
                min_y = position.y;
            }
        }

    float x = min_x + ((max_x - min_x) /2.f);
    float y = min_y + ((max_y - min_y) /2.f);
    return {x,y};

    }


    //check if players collide with any limbs
    //returns 1 if an arm collides with player 1
    //returns 2 if an arm collides with player 2
    //returns 3 if both players collides with an arm
    int LimbsManager::check_collision_with_players(Player1 *m_player1, Player2 *m_player2)
    {

    std::vector<Legs>::iterator itL;
    int leg_collided = 0;
    for (itL = m_legs.begin(); itL != m_legs.end();)
        {
            leg_collided = 0;
            if (m_player1->collides_with(*itL)) {
                
                m_player1->increase_speed();
                leg_collided = 1;
            }
            if (m_player2->collides_with(*itL)){
               m_player2->increase_speed();
               leg_collided = 1;
            }
            
            if(leg_collided != 0) {
                //erase.push_back(armcount);
                itL = m_legs.erase(itL);
                itL->destroy();
            } else {
                ++itL;
            }

        }


    std::vector<Arms>::iterator itA;
    int collided = 0;

        for (itA = m_arms.begin(); itA != m_arms.end();)
        {
            if (m_player1->collides_with(*itA)) {
                if (collided == 0) {
                    collided = 1;
                } else if (collided == 2)
                {
                    collided = 3;
                }
                
            }
            if (m_player2->collides_with(*itA)){
                if (collided == 0) {
                    collided = 2;
                } else if (collided == 1)
                {
                    collided = 3;
                }
            }

            if (collided != 0)
            {
                //erase.push_back(armcount);
                itA = m_arms.erase(itA);
                itA->destroy();
            } else {
                ++itA;
            }
            
        }

    update_clusters();
    return collided;
    }

   //check if any limbs collided, and if yes, combine them into one
    void LimbsManager::checkLimbCollisions()
    {}
   //if collide, delete rest keep 1, update 1's value respectively


    size_t LimbsManager::get_arms_size()
    {
        return m_arms.size();
    }

    size_t LimbsManager::get_legs_size()
    {
        return m_legs.size();
    }


    void LimbsManager::destroy()
    {
    for (auto& arm : m_arms)
        arm.destroy();
    for (auto& leg : m_legs)
        leg.destroy();

        m_arms.clear();
        m_legs.clear();
    }


