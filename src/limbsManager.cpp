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
        pair_legs();
        pair_arms();
        cluster_limbs();
        // update_limb_targets();

    }

    //find most optimal clusters of legs and updates the mapping using k-means
    void LimbsManager::pair_legs()
    {
        std::list<Legs*> leg_pointers;
        for(Legs l : m_legs)
        {
            leg_pointers.push_back(&l);
        }
        
        std::list<Legs*>::iterator it;
        for (it = leg_pointers.begin(); it != leg_pointers.end();)
        {
            std::list<Legs*>::iterator it2;
            it2 = leg_pointers.begin();
            it2++;

            Legs* curr_pair = NULL;
            double curr_min_distance = std::numeric_limits<double>::max();

            for (it2 = it2 ; it2 != leg_pointers.end();)
            {
                if(getDistance((*it)->get_position(), (*it2)->get_position()) < curr_min_distance)
                {
                    curr_min_distance = getDistance((*it)->get_position(), (*it2)->get_position());
                    curr_pair = *it2;
                }
            }
    
        m_leg_pairs.insert(std::pair<Legs*, Legs*>(*it, curr_pair));
        leg_pointers.remove(*it);
        leg_pointers.remove(curr_pair);
        }
        
        leg_pointers.clear();
    }
    //check if leg centroids are empty
    //if empty, find random points
    //if not empty, use points to find new clustering of legs

    //find most optimal clusters of arms and updates the mapping using k-means
    void LimbsManager::pair_arms()
    {
        std::list<Arms*> arm_pointers;
        for(Arms a : m_arms)
        {
            arm_pointers.push_back(&a);
        }
        
        std::list<Arms*>::iterator it;
        for (it = arm_pointers.begin(); it != arm_pointers.end();)
        {
            std::list<Arms*>::iterator it2;
            it2 = arm_pointers.begin();
            it2++;

            Arms* curr_pair = NULL;
            double curr_min_distance = std::numeric_limits<double>::max();

            for (it2 = it2 ; it2 != arm_pointers.end();)
            {
                if(getDistance((*it)->get_position(), (*it2)->get_position()) < curr_min_distance)
                {
                    curr_min_distance = getDistance((*it)->get_position(), (*it2)->get_position());
                    curr_pair = *it2;
                }
            }
    
        m_arm_pairs.insert(std::pair<Arms*, Arms*>(*it, curr_pair));
        arm_pointers.remove(*it);
        arm_pointers.remove(curr_pair);
        }
        
        arm_pointers.clear();
    }
      //check if leg centroids are empty
    //if empty, find random points
    //if not empty, use points to find new clustering of legs

    //find most optimal clusters of leg clusters and arm clusters to make a common enemy zombie using k-means
    void LimbsManager::cluster_limbs()
    {
        std::map<Legs*, Legs*>::iterator itL;
        for (itL = m_leg_pairs.begin(); itL != m_leg_pairs.end();)
        {
            std::map<Arms*, Arms*>::iterator itA;

            std::pair<Arms*, Arms*> curr_pair;
            double curr_min_distance = std::numeric_limits<double>::max();

            for (itA = m_arm_pairs.begin(); itA != m_arm_pairs.end();)
            {
                if(distance_between_pairs(*itL, *itA) < curr_min_distance)
                {
                    curr_min_distance = distance_between_pairs(*itL, *itA);
                    curr_pair = *itA;
                }
            }

            if(curr_pair.empty()) {
                vec2 target = get_centroid(*itL, curr_pair);
                (std::get<0>(*itL))->setCurrentTarget(target);
                (std::get<1>(*itL))->setCurrentTarget(target);
                (std::get<0>(curr_pair)).setCurrentTarget(target);
                (std::get<1>(curr_pair)).setCurrentTarget(target);

                m_leg_pairs.erase(itL);
                m_arm_pairs.erase(curr_pair);
            }

        }
        
        if(!m_arm_pairs.empty()) {
            for(std::pair<Arms*, Arms*> a : m_arm_pairs)
            {
                Arms* a1 = std::get<0>(a);
                Arms* a2 = std::get<1>(a);
                
                float x = ((a1->get_position()).x + (a2->get_position()).x)/2;
                float y = ((a1->get_position()).y + (a2->get_position()).y)/2;

                vec2 midpoint = {x, y};

                a1->setCurrentTarget(midpoint);
                a2->setCurrentTarget(midpoint);
            
            }
        }
        if(!m_leg_pairs.empty()) {
            for(std::pair<Legs*, Legs*> l : m_leg_pairs)
            {
                Legs* l1 = std::get<0>(l);
                Legs* l2 = std::get<1>(l);
                
                float x = ((l1->get_position()).x + (l2->get_position()).x)/2
                float y = ((l1->get_position()).y + (l2->get_position()).y)/2

                vec2 midpoint = {x, y};

                l1->setCurrentTarget(midpoint);
                l2->setCurrentTarget(midpoint);
            }
        }

    }
    //check if limbs centroids are empty
    //if empty, find random points
    //if not empty, use points to find new clustering of limbs
    //once new centroids are found, update the targetNode of each limb
    //if new centroid is further from limb than the current targetNode, abort. set the far limb to also go to the closest centroid.



    double distance_between_pairs(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a)
    {
            //check for null pointers
            return 0.0;

    }

    vec2 get_centroid(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a)
    {
        std::list<vec2> positions;
        if (std::get<0>(l)) != NULL)
        {
            positions.push_back(std::get<0>(l)).get_position());
        }

        if (std::get<1>(l)) != NULL)
        {
            positions.push_back(std::get<1>(l)).get_position());
        }

        if (std::get<0>(a)) != NULL)
        {
            positions.push_back(std::get<0>(a).get_position());
        }

        if (std::get<1>(a))) != NULL)
        {
            positions.push_back(std::get<1>(a)).get_position());
        }

        float max_x = NULL;
        float max_y = NULL;
        float min_x = NULL;
        float min_y = NULL;

        for (vec2 position : positions)
        {
            if (max_x == NULL)
            {
                max_x = position.x;
            } else if (max_x < position.x) {
                max_x = position.x;
            }

            if (max_y == NULL)
            {
                max_y = position.y;
            } else if (max_y < position.y) {
                max_y = position.y;
            }

            if (min_x == NULL)
            {
                min_x = position.x;
            } else if (min_x > position.x) {
                min_x = position.x;
            }

            if (min_y == NULL)
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
   
    void update_limb_targets()
    {

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


