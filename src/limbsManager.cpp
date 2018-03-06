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
        arm.setCurrentTarget({100, 300});
        arm.setLastTarget(arm.getCurrentTarget());

        // Setting random initial position
        arm.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
                          ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

        m_arms.emplace_back(arm);

        cluster_limbs();
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
bool LimbsManager::spawn_legs() {
    Legs leg;
    srand((unsigned) time(0));
    if (leg.init()) {
        leg.setCurrentTarget({100, 300});
        leg.setLastTarget(leg.getCurrentTarget());

        // Setting random initial position
        leg.set_position({((rand() % (int) m_screen.x) * ViewHelper::getRatio()),
                          ((rand() % (int) m_screen.y) * ViewHelper::getRatio())});

        m_legs.emplace_back(leg);

        cluster_limbs();
        return true;
    }
    fprintf(stderr, "Failed to spawn leg");
    return false;
}

void LimbsManager::cluster_limbs() {
    if ((m_arms.size() + m_legs.size()) > 1) {
        std::vector<Point> points;
        for (int i = 0; i < m_arms.size(); ++i) {
            std::vector<float> values = {m_arms[i].get_position().x, m_arms[i].get_position().x};
            Point p(i, values);
            points.emplace_back(p);
        }

        for (int j = 0; j < m_legs.size(); ++j) {
            std::vector<float> values = {m_legs[j].get_position().x, m_legs[j].get_position().x};
            Point p(j * m_arms.size(), values);
            points.emplace_back(p);
        }

        KMeans THE_CLUSTERIZER((int) (points.size() + 3) / 4, (int) points.size(), 2, MAX_ITERATIONS);
        THE_CLUSTERIZER.run(points);

        for (auto cluster : THE_CLUSTERIZER.getClusters()) {
            float cluster_x = cluster.getCentralValue(0);
            float cluster_y = cluster.getCentralValue(1);

            for (auto point : cluster.getPoints()) {
                int id = point.getID();
                if (id >= m_arms.size()) {
                    m_legs[id / m_arms.size()].move({cluster_x, cluster_x});
                } else {
                    m_arms[id].move({cluster_x, cluster_y});
                }
            }
        }
    }
}


//check if players collide with any limbs
//returns 1 if an arm collides with player 1
//returns 2 if an arm collides with player 2
//returns 3 if both players collides with an arm
int LimbsManager::check_collision_with_players(Player1 *m_player1, Player2 *m_player2) {

    std::vector<Legs>::iterator itL;
    int leg_collided = 0;
    for (itL = m_legs.begin(); itL != m_legs.end();) {
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
            itL->destroy();
        } else {
            ++itL;
        }
    }


    std::vector<Arms>::iterator itA;
    int collided = 0;

    for (itA = m_arms.begin(); itA != m_arms.end();) {
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