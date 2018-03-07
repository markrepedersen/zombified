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
    for (auto &arms : m_arms)
        arms.draw(projection_2D);
    for (auto &legs : m_legs)
        legs.draw(projection_2D);
}

//spawn new arm in random
bool LimbsManager::spawn_arms() {
    srand((unsigned) time(0));

    Arms arm;
    if (arm.init()) {
        // Setting random initial position
        arm.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
                          ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

        m_arms.emplace_back(arm);
        limbs.emplace_back(arm);

        return cluster_limbs();
    }
    fprintf(stderr, "Failed to spawn arm");
    return false;
}

//spawn new leg in random
bool LimbsManager::spawn_legs() {
    Legs leg;
    srand((unsigned) time(0));
    if (leg.init()) {
        // Setting random initial position
        leg.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
                          ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

        m_legs.emplace_back(leg);
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

        for (auto cluster : THE_CLUSTERIZER.getClusters()) {
            auto cluster_x = (int) cluster.getCentralValue(0) % (int) m_screen.x;
            auto cluster_y = (int) cluster.getCentralValue(1) % (int) m_screen.y;

            for (auto point : cluster.getPoints()) {
                int id = point.getID();
                limbs[id].setCurrentTarget({static_cast<float>(cluster_x), static_cast<float>(cluster_y)});
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

    int leg_collided = 0;
    for (auto itL = m_legs.begin(); itL != m_legs.end();) {
        leg_collided = 0;
        if (m_player1->collides_with(*itL)) {

            m_player1->increase_speed();
            leg_collided = 1;
        }
        if (m_player2->collides_with(*itL)) {
            m_player2->increase_speed();
            leg_collided = 1;
        }

        if (leg_collided != 0) {
            //erase.push_back(armcount);
            itL = m_legs.erase(itL);
            limbs.erase(itL);
            itL->destroy();
        } else {
            ++itL;
        }
    }


    int collided = 0;

    for (auto itA = m_arms.begin(); itA != m_arms.end();) {
        if (m_player1->collides_with(*itA)) {
            if (collided == 0) {
                collided = 1;
            } else if (collided == 2) {
                collided = 3;
            }
        }
        if (m_player2->collides_with(*itA)) {
            if (collided == 0) {
                collided = 2;
            } else if (collided == 1) {
                collided = 3;
            }
        }

        if (collided != 0) {
            cluster_limbs();
            //erase.push_back(armcount);
            itA = m_arms.erase(itA);
            limbs.erase(itA);
            itA->destroy();
        } else {
            ++itA;
        }

    }
    return collided;
}

size_t LimbsManager::get_arms_size() {
    return m_arms.size();
}

size_t LimbsManager::get_legs_size() {
    return m_legs.size();
}


void LimbsManager::destroy() {
    for (auto &arm : m_arms)
        arm.destroy();
    for (auto &leg : m_legs)
        leg.destroy();

    m_arms.clear();
    m_legs.clear();
}