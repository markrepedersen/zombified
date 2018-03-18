// Header
#include "limbsManager.hpp"
#include "KMeans.h"

#define MAX_ITERATIONS 100

// initialize a limbsManager
bool LimbsManager::init(vec2 screen, const std::vector<vec2> &mapCollisionPoints) {
    randomPoints = mapCollisionPoints;
    m_screen = screen;
    return true;
}

// Renders the existing limbs
void LimbsManager::draw(const mat3 &projection_2D) {
    for (auto &limb : limbs) {
        limb.draw(projection_2D);
    }
    for (auto &collectedLegs1 : collectedLegs_p1) {
        collectedLegs1.draw(projection_2D);
    }
    for (auto &collectedLegs2 : collectedLegs_p2) {
        collectedLegs2.draw(projection_2D);
    }
}

vec2 LimbsManager::getRandomPointInMap() {

    srand((unsigned)time(0));

    vec2 randomPoint = {(float)((rand() % (int)m_screen.x)),
                        (float)((rand() % (int)m_screen.y))};

    while(!isInsidePolygon(randomPoints, randomPoint)) {
        randomPoint = {(float)((rand() % (int)m_screen.x)),
                       (float)((rand() % (int)m_screen.y))};

    }

    return randomPoint;

}

//spawn new arm in random
bool LimbsManager::spawn_arms() {
    Limb arm;
    if (arm.init("arm")) {
        arm.set_position(getRandomPointInMap());
        m_arms_total++;
        limbs.emplace_back(arm);

        return cluster_limbs();
    }

    return false;
}

//spawn new leg in random
bool LimbsManager::spawn_legs() {
    Limb leg;
    if (leg.init("leg")) {
        leg.set_position(getRandomPointInMap());

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

std::vector<Limb> LimbsManager::getCollectedLegs(int player) {
    if (player == 1)
        return collectedLegs_p1;
    else
        return collectedLegs_p2;
}

void LimbsManager::decreaseCollectedLegs(int player) {
    if (player == 1)
    {
        collectedLegs_p1.begin()->destroy();
        collectedLegs_p1.erase(collectedLegs_p1.begin());
    }
    if (player == 2)
    {
        collectedLegs_p2.begin()->destroy();
        collectedLegs_p2.erase(collectedLegs_p2.begin());
    }
}

void LimbsManager::shiftCollectedLegs(int player, ToolboxManager *m_toolboxManager, float index, int legcount) {
    if (player == 1)
    {
        Limb &legs = collectedLegs_p1.at(legcount);
        legs.set_position(m_toolboxManager->new_tool_position(index, 1));
    }
    if (player == 2) {
        Limb &legs = collectedLegs_p2.at(legcount);
        legs.set_position(m_toolboxManager->new_tool_position(index, 2));
    }
}

//check if players collide with any limbs
//returns 1 if an arm collides with player 1
//returns 2 if an arm collides with player 2
//returns 3 if both players collides with an arm
int LimbsManager::check_collision_with_players(Player1 *m_player1, Player2 *m_player2, ToolboxManager *m_toolboxmanager) {
//    printf("Checking Collision: #Limbs %d\n", limbs.size());
    int collided = 0;
    for (auto it = limbs.begin(); it != limbs.end();) {
         int limb_collided = 0;
        
        if (m_player1->collides_with(*it))
            collided = 1;
        if (m_player2->collides_with(*it))
            collided = 2;
        
        if (collided != 0)
        {
            if ((*it).getLimbType() == "leg") {
                float index = (float)m_toolboxmanager->addItem(4, collided);
                if ((int)index != 100)
                {
                    //it->destroy();
                    if(collided == 1)
                    {
                        m_player1->increase_speed_legs(10);
                        //m_player1->set_mass(it->get_mass()+m_player1->get_mass());
                        collectedLegs_p1.emplace_back(*it);
                        Limb &new_leg = collectedLegs_p1.back();
                        new_leg.set_position(m_toolboxmanager->new_tool_position(index, collided));
                        collided = 0;
                        //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                    }
                    if (collided == 2)
                    {
                        m_player2->increase_speed_legs(10);
                       // m_player2->set_mass(it->get_mass()+m_player2->get_mass());
                        collectedLegs_p2.emplace_back(*it);
                        Limb &new_leg = collectedLegs_p2.back();
                        new_leg.set_position(m_toolboxmanager->new_tool_position(index, collided));
                        collided = 0;
                        //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                    }
                    it = limbs.erase(it);
                    m_legs_total--;
                }
                else
                    ++it;
                
            }
        
            if ((*it).getLimbType() == "arm") {
                if(m_toolboxmanager->addSlot(collided))
                {
                    //erase.push_back(armcount);
                    it->destroy();
                    it = limbs.erase(it);
                    m_arms_total--;
                }
                else
                    ++it;
            }
        }
        else
            ++it;
    }

//        if (m_player1->collides_with(*it)) {
//
//            if ((*it).getLimbType() == "leg") {
//                m_player1->increase_speed_legs(10);
//                m_legs_total--;
//            } else {
//                if (collided == 0) {
//                    collided = 1;
//                } else if (collided == 2) {
//                    collided = 3;
//                }
//
//                m_arms_total--;
//            }
//
//            limb_collided = 1;
//        }
//        if (m_player2->collides_with(*it)) {
//            if ((*it).getLimbType() == "leg") {
//                m_player2->increase_speed_legs(10);
//                m_legs_total--;
//            } else {
//                if (collided == 0) {
//                    collided = 2;
//                } else if (collided == 2) {
//                    collided = 3;
//                }
//
//                m_arms_total--;
//            }
//            limb_collided = 1;
//        }
//
//        if (limb_collided != 0) {
//            it->destroy();
//            it = limbs.erase(it);
//        } else {
//            ++it;
//        }
    return collided;

}

int LimbsManager::get_arms_size() {
    // return m_arms.size();
    return m_arms_total;
}

int LimbsManager::get_legs_size() {
    return m_legs_total;
}


void LimbsManager::computePaths(float ms, const MapGrid &mapGrid) {
    if (limbs.size() <= 1) return;
    for (auto &limb : limbs) {
        JPS::PathVector path;
        vec2 target = limb.getCurrentTarget();

        if (limb.getLastTarget() != target || !limb.isInitialized()) {
            auto srcX = (unsigned) (limb.get_position().x / 100);
            auto srcY = (unsigned) (limb.get_position().y / 100);
            auto dstX = (unsigned) (target.x / 100);
            auto dstY = (unsigned) (target.y / 100);
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
            dir.x = nextNode.x * 100 - limb.get_position().x;
            dir.y = nextNode.y * 100 - limb.get_position().y;

            auto next_pos = scale(step, normalize(dir));

            limb.move(next_pos);
            limb.setLastPath(limb.getCurrentPath());
            limb.setLastTarget(target);
        }
    }
}

void LimbsManager::destroy() {
    for (auto &limb : limbs)
        limb.destroy();

    limbs.clear();
}