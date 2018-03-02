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
        srand((unsigned)time(0));

        Arms arm;
        if (arm.init())
        {
            arm.setCurrentTarget({100,300});
            arm.setLastTarget(arm.getCurrentTarget());

            // Setting random initial position
            arm.set_position({(float)((rand() % (int)m_screen.x) * ViewHelper::getRatio()),
                (float)((rand() % (int)m_screen.y) * ViewHelper::getRatio())});

            m_arms.emplace_back(arm);
           
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
     srand((unsigned)time(0));
    if (leg.init())
    {
        leg.setCurrentTarget({100,300});
        leg.setLastTarget(leg.getCurrentTarget());
        
        // Setting random initial position
        leg.set_position({(float)((rand() % (int)m_screen.x) * ViewHelper::getRatio()),
            (float)((rand() % (int)m_screen.y) * ViewHelper::getRatio())});

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
        update_limb_targets();

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
     
     
//cluster_limbs:
      //if temp nodes not empty
       //find closest from temp nodes that does not have a full cluster
        //check if the other node has a positive cluster key,
            //if yes, add to that cluster on cluster map
                //if this makes 4, remove all those node pointers pointing to nodes from temp lsit
            //if no, create new cluster and add to cluster map
        //if temp nodes empty, set as its own cluster
    void LimbsManager::cluster_limbs()
    {
        
        //vector of all nodes
        std::vector<struct Node> limb_nodes;

        //temp list of nodes pointers that are not in full clusters
        std::list<struct Node*> temp_nodes;

        //initialize nodes from limbs with cluster -1   
        for (Arms a : m_arms)
        {
            Node n = {-1, &a, a.get_position()};
            limb_nodes.push_back(n);
            temp_nodes.push_back(&n);
        }
        for (Legs l : m_legs)
        {
            Node n = {-1, &l, l.get_position()};
            limb_nodes.push_back(n);
            temp_nodes.push_back(&n);
        }

        // for (struct Node* n : temp_nodes)
        // {
        //     std::cout << "start of node" << std::endl;
        //     std::cout<< n->limb_position.x << std::endl;
        //     std::cout<< n->limb_position.y << std::endl;
        //     std::cout<< "done" << std::endl;
        // }
        // std::cout << "end of loop";
       
       m_clusters.clear();
       std::cout << "number of nodes in this round is..." << limb_nodes.size() << std::endl;
       //for each node in limb_nodes
       for(struct Node ln : limb_nodes)
       {

            struct Node* temp_closest_node;
            bool temp_closest_node_empty = true;
            if (ln.cluster_key == -1) {
            if (!temp_nodes.empty())
            {
                float temp_closest_distance = 10000.0;

                for (struct Node* n : temp_nodes)
                {
                    //TODO:: check if node is not itself
                    float distance = getDistance(ln.limb_position, n->limb_position);
                    if (distance < temp_closest_distance)
                    {
                        temp_closest_node_empty = false;
                        temp_closest_node = n;
                        temp_closest_distance = distance;
                    }
                }

           }

           if(temp_closest_node_empty)
           {
               std::cout<< "temp_closest node is empty" << std::endl;
                int curr_cluster_key = m_clusters.size() + 1;
                std::vector<struct Node*> cluster_limbs;
                cluster_limbs.push_back(&ln);
                ln.cluster_key = curr_cluster_key;
                struct Cluster c = {1, cluster_limbs};
                m_clusters[curr_cluster_key] = c;

           } else {
                if (temp_closest_node->cluster_key == -1)
                    {
                        std::cout << "closest node has no clusters assigned" << std::endl;
                        int curr_cluster_key = m_clusters.size() + 1;
                        std::vector<struct Node*> cluster_limbs;
                        cluster_limbs.push_back(temp_closest_node);
                        temp_closest_node->cluster_key = curr_cluster_key;
                        cluster_limbs.push_back(&ln);
                        ln.cluster_key = curr_cluster_key;
                        struct Cluster c = {2, cluster_limbs};
                        m_clusters[curr_cluster_key] = c;
                    } else if ((m_clusters[temp_closest_node->cluster_key]).size < 4)
                    {
                        std::cout<< "closest node has a cluster assigned that's not full"<< std::endl;
                        (m_clusters[temp_closest_node->cluster_key]).nodes.push_back(&ln);
                        (m_clusters[temp_closest_node->cluster_key]).size++;
                        ln.cluster_key = temp_closest_node->cluster_key;

                        if ((m_clusters[temp_closest_node->cluster_key]).size == 4)
                        {
                            for(struct Node* full_cluster_node : (m_clusters[temp_closest_node->cluster_key]).nodes)
                            {
                                temp_nodes.remove(full_cluster_node);
                            }
                        }
                    }
           }
           std::cout << "one limb down" <<std::endl;
            }
       }
       std::cout << "looked through all the limbs in this round"<< std::endl;
      


        //TODO::
        //go through all clusters, find centroids
        //update targets for each limb
        for (std::map<int, LimbsManager::Cluster>::iterator it_clusters = m_clusters.begin();
        it_clusters != m_clusters.end(); ++it_clusters)
        {
            std::cout << "printing a cluster size";
            std::cout << (it_clusters->second).size << std::endl;
        }

        std::cout << "done this round" << std::endl;
      
            
        

    }
    

    void LimbsManager::update_limb_targets()
    {
        //iterate through all clusters, find its centroids, update targets of each limb
    }

    double LimbsManager::distance_between_pairs(std::pair<Legs*, Legs*> l, std::pair<Arms*, Arms*> a)
    {

            //check for null pointers
            return 0.0;

    }

    vec2 LimbsManager::get_centroid(std::pair<Legs *, Legs *> pair1, std::pair<Arms *, Arms *> pair2) {
        vec2 pos1 = pair1.first->get_position();
        vec2 pos2 = pair1.second->get_position();
        vec2 pos3 = pair2.first->get_position();
        vec2 pos4 = pair2.second->get_position();
        return {(pos1.x + pos2.x + pos3.x + pos4.x) / 4, (pos1.y + pos2.y + pos3.y + pos4.y) / 4};
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
                update_clusters();
                //erase.push_back(armcount);
                itA = m_arms.erase(itA);
                itA->destroy();
            } else {
                ++itA;
            }
            
        }
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


