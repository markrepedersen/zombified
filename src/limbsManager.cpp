// Header
#include "limbsManager.hpp"
#include "KMeans.h"

#define MAX_ITERATIONS 100

// initialize a limbsManager
bool LimbsManager::init(vec2 screen) {
    m_screen = screen;
    return true;
}

// Renders the existing limbs
void LimbsManager::draw(const mat3 &projection_2D) {
    for (auto &limb : limbs) {
        limb.draw(projection_2D);
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
            auto cluster_x = (int) cluster.getCentralValue(0);
            auto cluster_y = (int) cluster.getCentralValue(1);

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
int limb_collided = 0;
    int collided;
    for (auto it = limbs.begin(); it != limbs.end();) {

        if (m_player1->collides_with(*it)) {

            if ((*it).getLimbType() == "leg") {
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
            if ((*it).getLimbType() == "leg") {
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
    return m_legs_total;
}


void LimbsManager::computePaths(float ms, MapGrid const mapGrid) {
    if (limbs.size() <= 1) return;
    for (auto &limb : limbs) {
        JPS::PathVector path;
        vec2 target = limb.getCurrentTarget();

        if (limb.getLastTarget() != target || !limb.isInitialized()) {
            auto srcX = (unsigned) (limb.get_position().x/100);
            auto srcY = (unsigned) (limb.get_position().y/100);
            auto dstX = (unsigned) (target.x/100);
            auto dstY = (unsigned) (target.y/100);
            JPS::findPath(path, mapGrid, srcX, srcY, dstX, dstY, 1);
            limb.setCurrentPath(path);
            limb.setInitialized(true);
        } else limb.setCurrentPath(limb.getLastPath());
        if (!limb.getCurrentPath().empty()) {
            vec2 nextNode, curNode;
            curNode = nextNode = {limb.get_position().x, limb.get_position().y};

            for (int i = 0; i < limb.getCurrentPath().size() && getDistance(curNode, nextNode) < 10; ++i) {
                nextNode = {static_cast<float>(limb.getCurrentPath()[i].x),
                            static_cast<float>(limb.getCurrentPath()[i].y)};
            }
            float step = 50 * (ms / 1000);
            vec2 dir;
            dir.x = nextNode.x*100 - limb.get_position().x;
            dir.y = nextNode.y*100 - limb.get_position().y;

            auto next_pos = scale(step, normalize(dir));

            limb.move(next_pos);
            limb.setLastPath(limb.getCurrentPath());
            limb.setLastTarget(target);
        }
    }
}

void LimbsManager::destroy() {
    for(auto &limb : limbs)
        limb.destroy();

    limbs.clear();
}

void LimbsManager::processCollisions(vec2 pos) {

}