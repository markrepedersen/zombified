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
    // std::cout << "draw" << std::endl;
    // int i = 0;
    for (auto &limb : limbs)
    {
        // std::cout << i << " " <<  limb.getCurrentPath().size() << std::endl;
        limb.draw(projection_2D);
        // i++;
    }
}

//spawn new arm in random
bool LimbsManager::spawn_arms() {
    srand((unsigned) time(0));

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
    return m_arms_total;
}

int LimbsManager::get_legs_size() {
    return m_legs_total;
}


void LimbsManager::computePaths(float ms, MapGrid const mapGrid) {

std::cout << "computePaths" << std::endl;
    for (int k = 0 ; k < limbs.size() ; k++) {

        JPS::PathVector path;
        vec2 target = limbs[k].getCurrentTarget();
        printf("<<<<<<<<<<<<<<<<<<<<\n");
        std::cout << "the k is: " << k << std::endl;
        printf("target x: %f, target y: %f\n", target.x, target.y);
        
        // std::cout<< "this one limb" << limb.get_position().x <<std::endl;
// std::cout << k << "123" << limbs[k].get_position().x << std::endl;
        if (limbs[k].getLastTarget() != target) {
            // if (limbs[k].getLastTarget() == (vec2) {0, 0}) {
            std::cout << "case 1: new target or last target empty" << std::endl;
            JPS::findPath(path,
                          mapGrid,
                          (unsigned) limbs[k].get_position().x,
                          (unsigned) limbs[k].get_position().y,
                          (unsigned) target.x,
                          (unsigned) target.y,
                          1);

                          printf("now setting up new path yay\n");
            limbs[k].setCurrentPath(path);
            // }
            // else {

            // }

            // std::cout << "size of current path is: " << limbs[k].getCurrentPath().size() << std::endl;
        } else {
            std::cout << "case2: no new target" << std::endl;
            std::cout << "size of last path is: " << limbs[k].getLastPath().size() << std::endl;
            // if (!limbs[k].getLastPath().empty()) {            
                limbs[k].setCurrentPath(limbs[k].getLastPath());
            // } else {
            //     JPS::findPath(path,
            //               mapGrid,
            //               (unsigned) limbs[k].get_position().x,
            //               (unsigned) limbs[k].get_position().y,
            //               (unsigned) target.x,
            //               (unsigned) target.y,
            //               1);  
            // // limbs[k].setCurrentPath(path);
            // }
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
    for(auto &limb : limbs)
        limb.destroy();

    limbs.clear();
}