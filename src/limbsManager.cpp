// Header
#include "limbsManager.hpp"
#include "KMeans.h"

#include <cmath>

#define MAX_ITERATIONS 100

// initialize a limbsManager
bool LimbsManager::init(vec2 screen) {
    m_screen = screen;
    return true;
}

// Renders the existing limbs
void LimbsManager::draw(const mat3 &projection_2D) {
    // for (auto &arms : m_arms)
    // {
    //     // std::cout<<arms.get_position().x <<std::endl;
    //     arms.draw(projection_2D);
    // }
    // for (auto &legs : m_legs)
    // {
    //     // std::cout<<legs.get_position().x <<std::endl;
    //     legs.draw(projection_2D);
    // }

    // std::cout << "draw" << std::endl;
    // int i = 0;
    for (auto &limb : limbs)
    {
        // std::cout << i << " " <<  limb.getCurrentPath().size() << std::endl;
        limb.draw(projection_2D);
        // i++;
    }


//    for (auto limb : limbs)
//     {
//         if (typeid(&limb) == typeid(Arms)) {
//             Arms* arm = dynamic_cast<Arms*>(&limb);
//             arm->draw(projection_2D);
//         } else {
//             Legs* leg = dynamic_cast<Legs*>(&limb);
//             leg->draw(projection_2D);
//         }
//     }
}

//spawn new arm in random
bool LimbsManager::spawn_arms() {
    srand((unsigned) time(0));

    // Arms arm;
    // if (arm.init()) {
    //     // Setting random initial position
    //     arm.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
    //                       ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

    //     m_arms.emplace_back(arm);
    //     limbs.emplace_back(arm);

    //     return cluster_limbs();
    // }
    Limb arm;
    if (arm.init("arm")) {
        // Setting random initial position
        arm.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
                          ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

        m_arms_total++;
        limbs.emplace_back(arm);

        return cluster_limbs();
    }

    fprintf(stderr, "Failed to spawn arm");
    return false;
}

//spawn new leg in random
bool LimbsManager::spawn_legs() {
    // Legs leg;
    // srand((unsigned) time(0));
    // if (leg.init()) {
    //     // Setting random initial position
    //     leg.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
    //                       ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

    //     m_legs.emplace_back(leg);
    //     limbs.emplace_back(leg);

    //     return cluster_limbs();
    // }

        Limb leg;
    srand((unsigned) time(0));
    if (leg.init("leg")) {
        // Setting random initial position
        leg.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
                          ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

        m_legs_total++;
        limbs.emplace_back(leg);

        return cluster_limbs();
    }


    fprintf(stderr, "Failed to spawn leg");
    return false;
}

bool LimbsManager::cluster_limbs() {
    if (limbs.size() > 1) {
        std::vector<Point> points;
        for (int i = 0; i < limbs.size(); ++i) {
            std::vector<float> values = {limbs[i].get_position().x, limbs[i].get_position().y};
            Point p(i, values);
            points.emplace_back(p);
        }

        KMeans THE_CLUSTERIZER((int) (points.size() + 3) / 4, (int) points.size(), 2, MAX_ITERATIONS);
        THE_CLUSTERIZER.run(points);

        std::cout << "clustering limbs" << std::endl;

        for (auto cluster : THE_CLUSTERIZER.getClusters()) {
            auto cluster_x = (int) cluster.getCentralValue(0) % (int) 1280;
            auto cluster_y = (int) cluster.getCentralValue(1) % (int) 720;

            for (auto point : cluster.getPoints()) {
                int id = point.getID();
                limbs[id].setCurrentTarget({static_cast<float>(cluster_x), static_cast<float>(cluster_y)});
                // std::cout << id << " " << cluster_x << " " << cluster_y << std::endl;

            }
        }
    }
    return true;
}


//check if players collide with any limbs
//returns 1 if an arm collides with player 1
//returns 2 if an arm collides with player 2
//returns 3 if both players collides with an arm
int LimbsManager::check_collision_with_players(Player1 *m_player1, Player2 *m_player2) {

    // int leg_collided = 0;
    // for (auto itL = m_legs.begin(); itL != m_legs.end();) {
    //     leg_collided = 0;
    //     if (m_player1->collides_with(*itL)) {

    //         m_player1->increase_speed();
    //         leg_collided = 1;
    //     }
    //     if (m_player2->collides_with(*itL)) {
    //         m_player2->increase_speed();
    //         leg_collided = 1;
    //     }

    //     if (leg_collided != 0) {
    //         //erase.push_back(armcount);
    //         itL = m_legs.erase(itL);
    //         limbs.erase(itL);
            
    //         itL->destroy();
    //     } else {
    //         ++itL;
    //     }
    // }


    // int collided = 0;

    // for (auto itA = m_arms.begin(); itA != m_arms.end();) {
    //     if (m_player1->collides_with(*itA)) {
    //         if (collided == 0) {
    //             collided = 1;
    //         } else if (collided == 2) {
    //             collided = 3;
    //         }
    //     }
    //     if (m_player2->collides_with(*itA)) {
    //         if (collided == 0) {
    //             collided = 2;
    //         } else if (collided == 1) {
    //             collided = 3;
    //         }
    //     }

    //     if (collided != 0) {
    //         cluster_limbs();
    //         //erase.push_back(armcount);
    //         itA = m_arms.erase(itA);
    //         limbs.erase(itA);
    //         itA->destroy();
    //     } else {
    //         ++itA;
    //     }

    // }
    // return collided;

    int limb_collided = 0;
    int collided;
    for (auto it = limbs.begin(); it != limbs.end();) {

        if (m_player1->collides_with(*it)) {

            if((*it).getLimbType() == "leg") {
                m_player1->increase_speed();
                m_legs_total--;
            } else {
                 if (collided == 0) {
                    collided = 1;
                } else if (collided == 2) {
                     collided = 3;
                }

                m_arms_total--;
            }

            limb_collided = 1;
        }
        if (m_player2->collides_with(*it)) {
             if((*it).getLimbType() == "leg") {
                m_player1->increase_speed();
                m_legs_total--;
            }  else {
                 if (collided == 0) {
                    collided = 1;
                } else if (collided == 2) {
                     collided = 3;
                }

                m_arms_total--;
            }
            limb_collided = 1;
        }

        if (limb_collided != 0) {
            //erase.push_back(armcount);
            it = limbs.erase(it);
            
            it->destroy();
        } else {
            ++it;
        }

    }

    return collided;

}

int LimbsManager::get_arms_size() {
    // return m_arms.size();
    return m_arms_total;
}

int LimbsManager::get_legs_size() {
    // return m_legs.size();
    return m_legs_total;
}


void LimbsManager::computePaths(float ms, MapGrid const mapGrid) {
std::cout << "computePaths" << std::endl;
    for (int k = 0 ; k < limbs.size() ; k++) {
        // JPS::PathVector path;
        // vec2 target = limb.getCurrentTarget();
        
        // // std::cout<< "this one limb" << limb.get_position().x <<std::endl;

        // if (limb.getLastTarget() != target || limb.getLastTarget() == (vec2) {0, 0}) {
        //     JPS::findPath(path,
        //                   mapGrid,
        //                   (unsigned) limb.get_position().x,
        //                   (unsigned) limb.get_position().y,
        //                   (unsigned) target.x,
        //                   (unsigned) target.y,
        //                   1);  
        //     limb.setCurrentPath(path);
        //     std::cout << "hello2" << std::endl;
        // } else limb.setCurrentPath(limb.getLastPath());
        // if (!limb.getCurrentPath().empty()) {
        //     vec2 nextNode, curNode;
        //     curNode = nextNode = {std::powf(limb.get_position().x, 2), std::powf(limb.get_position().y, 2)};

        //     for (int i = 0; i < limb.getCurrentPath().size() && curNode <= nextNode; ++i) {
        //         nextNode = {static_cast<float>(limb.getCurrentPath()[i].x),
        //                     static_cast<float>(limb.getCurrentPath()[i].y)};
        //     }
        //     float step = 200 * (ms / 1000);
        //     vec2 dir;
        //     dir.x = limb.getCurrentTarget().x - limb.get_position().x;
        //     dir.y = limb.getCurrentTarget().y - limb.get_position().y;

        //     auto jump = scale(step, normalize(dir));

        //     // printf("move: %f, %f\n", jump.x, jump.y);

        //     limb.move(jump);
        //     limb.setLastPath(limb.getCurrentPath());
        //     limb.setLastTarget(target);
        //     std::cout << j << std::endl;
        //     if (j == 4) {
        //     printf("move: %f, %f\n", jump.x, jump.y);
        //     std::cout << "this one limb" << j << limb.get_position().x <<std::endl;
        //     }
        // }

        JPS::PathVector path;
        vec2 target = limbs[k].getCurrentTarget();
        std::cout << "the k is: " << k << std::endl;
        std::cout << "the target is: " << target.x << std::endl;
        
        // std::cout<< "this one limb" << limb.get_position().x <<std::endl;
// std::cout << k << "123" << limbs[k].get_position().x << std::endl;
        if (limbs[k].getLastTarget() != target || limbs[k].getLastTarget() == (vec2) {0, 0}) {
            std::cout << "case 1: new target or last target empty" << std::endl;
            JPS::findPath(path,
                          mapGrid,
                          (unsigned) limbs[k].get_position().x,
                          (unsigned) limbs[k].get_position().y,
                          (unsigned) target.x,
                          (unsigned) target.y,
                          1);
            // limbs[k].setCurrentPath(path);

            // std::cout << "size of current path is: " << limbs[k].getCurrentPath().size() << std::endl;
        } else {
            std::cout << "case2: no new target" << std::endl;
            std::cout << "size of last path is: " << limbs[k].getLastPath().size() << std::endl;
            if (!limbs[k].getLastPath().empty()) {            
                limbs[k].setCurrentPath(limbs[k].getLastPath());
            } else {
                JPS::findPath(path,
                          mapGrid,
                          (unsigned) limbs[k].get_position().x,
                          (unsigned) limbs[k].get_position().y,
                          (unsigned) target.x,
                          (unsigned) target.y,
                          1);  
            // limbs[k].setCurrentPath(path);
            }
        }


            // limbs[k].setCurrentPath(path);
            // std::cout << "size of current path is: " << limbs[k].getCurrentPath().size() << std::endl;
            
        if (!limbs[k].getCurrentPath().empty()) {
            std::cout << "current path is not empty" << std::endl;
            vec2 nextNode, curNode;
            curNode = nextNode = {std::powf(limbs[k].get_position().x, 2), std::powf(limbs[k].get_position().y, 2)};

            for (int i = 0; i < limbs[k].getCurrentPath().size() && curNode <= nextNode; ++i) {
                nextNode = {static_cast<float>(limbs[k].getCurrentPath()[i].x),
                            static_cast<float>(limbs[k].getCurrentPath()[i].y)};
            }
            float step = 20 * (ms / 1000);
            vec2 dir;
            dir.x = limbs[k].getCurrentTarget().x - limbs[k].get_position().x;
            dir.y = limbs[k].getCurrentTarget().y - limbs[k].get_position().y;

            auto jump = scale(step, normalize(dir));

            // printf("move: %f, %f\n", jump.x, jump.y);

            limbs[k].move(jump);
            limbs[k].setLastPath(limbs[k].getCurrentPath());
            limbs[k].setLastTarget(target);
            std::cout << k << std::endl;
            printf("move: %f, %f\n", jump.x, jump.y);
            std::cout << "this one limb" << limbs[k].get_position().x <<std::endl;
        }
    }
}



void LimbsManager::destroy() {
    // for (auto &arm : m_arms)
    //     arm.destroy();
    // for (auto &leg : m_legs)
    //     leg.destroy();

    // m_arms.clear();
    // m_legs.clear();
    for(auto &limb : limbs)
        limb.destroy();

    limbs.clear();
}