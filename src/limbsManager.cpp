// Header
#include "limbsManager.hpp"
#include "KMeans.h"

#define MAX_ITERATIONS 100

// initialize a limbsManager
bool LimbsManager::init(vec2 screen, const std::vector<vec2> &mapCollisionPoints) {
    m_mapCollisionPoints = mapCollisionPoints;
    m_screen = screen;
    collectedLegs_p1 = 0;
    collectedLegs_p2 = 0;
    return true;
}

// Renders the existing limbs
void LimbsManager::draw(const mat3 &projection_2D) {
    for (auto &limb : limbs) {
        limb.draw(projection_2D);
    }
    /*for (auto &collectedLegs1 : collectedLegs_p1) {
        collectedLegs1.draw(projection_2D);
    }
    for (auto &collectedLegs2 : collectedLegs_p2) {
        collectedLegs2.draw(projection_2D);
    }*/
}

unsigned long LimbsManager::getLimbCount() {
    return limbs.size();
}

void LimbsManager::transformLimbs(std::vector<Renderable*> &container) {
    std::transform(limbs.begin(), limbs.end(), std::back_inserter(container), [](Limb& entity) { return &entity;});
}

//spawn new arm in random
bool LimbsManager::spawn_arms() {
    Limb arm;
    if (arm.init("arm")) {
        arm.set_position(getRandomPointInMap(m_mapCollisionPoints,
                                             {m_screen.x * ViewHelper::getRatio(),
                                              m_screen.y * ViewHelper::getRatio()}));
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
        leg.set_position(getRandomPointInMap(m_mapCollisionPoints,
                                             {m_screen.x * ViewHelper::getRatio(),
                                              m_screen.y * ViewHelper::getRatio()}));

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

int LimbsManager::getCollectedLegs(int player) {
    if (player == 1)
        return collectedLegs_p1;
    else
        return collectedLegs_p2;
}

void LimbsManager::decreaseCollectedLegs(int player) {
    if (player == 1)
    {
        collectedLegs_p1-= 1;
        //collectedLegs_p1.begin()->destroy();
        //collectedLegs_p1.erase(collectedLegs_p1.begin());
    }
    if (player == 2)
    {
        collectedLegs_p2-= 1;
        //collectedLegs_p2.begin()->destroy();
        //collectedLegs_p2.erase(collectedLegs_p2.begin());
    }
}

/*void LimbsManager::shiftCollectedLegs(int player, ToolboxManager *m_toolboxManager, float index, int legcount) {
    if (player == 1)
    {
        Limb &legs = collectedLegs_p1.at(legcount);
        legs.set_position(m_toolboxManager->new_tool_position(index, 1));
    }
    if (player == 2) {
        Limb &legs = collectedLegs_p2.at(legcount);
        legs.set_position(m_toolboxManager->new_tool_position(index, 2));
    }
}*/

//check if players collide with any limbs
//returns "1leg"
//returns "2leg"
//returns "else"
std::string LimbsManager::check_collision_with_players(Player1 *m_player1, Player2 *m_player2, ToolboxManager *m_toolboxmanager) {
//    printf("Checking Collision: #Limbs %d\n", limbs.size());
    string returnVal = "else";

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
                //float index = (float)m_toolboxmanager->addItem(4, collided);
                //if ((int)index != 100)
                //{
                    //it->destroy();
                    if(collided == 1)
                    {
                        returnVal = "1leg";
                        m_player1->increase_speed_legs(10);
                        //m_player1->set_mass(it->get_mass()+m_player1->get_mass());
                        //collectedLegs_p1.emplace_back(*it);
                        collectedLegs_p1 += 1;
                        //Limb &new_leg = collectedLegs_p1.back();
                        //new_leg.set_position(m_toolboxmanager->new_tool_position(index, collided));
                        //new_leg.legTime = time(0);
                        collided = 0;
                        //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                    }
                    if (collided == 2)
                    {
                        returnVal = "2leg";
                        m_player2->increase_speed_legs(10);
                       // m_player2->set_mass(it->get_mass()+m_player2->get_mass());
                        //collectedLegs_p2.emplace_back(*it);
                        //Limb &new_leg = collectedLegs_p2.back();
                        //new_leg.set_position(m_toolboxmanager->new_tool_position(index, collided));
                        collectedLegs_p2 += 1;
                        collided = 0;
                        //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                    }
                    it->destroy();
                    it = limbs.erase(it);
                    m_legs_total--;
                //}
                //else
                //    ++it;

            } else if ((*it).getLimbType() == "arm") {
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
    return returnVal;
}

int LimbsManager::get_arms_size() {
    // return m_arms.size();
    return m_arms_total;
}

int LimbsManager::get_legs_size() {
    return m_legs_total;
}

void LimbsManager::set_arms_size(int size) {
    m_arms_total = size;
}

void LimbsManager::set_legs_size(int size) {
    m_legs_total = size;
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

//check on clusters that already meet and spawn zombies there
std::unordered_set<vec2> LimbsManager::checkClusters() {
    std::map<vec2, int> zombie_map;
    std::unordered_set<vec2> zombie_set;

    //for each limb that has found its centroid, add to zombie_map
     for (auto it = limbs.begin(); it != limbs.end();) {
        if (getDistance(it->get_position(),it->getCurrentTarget()) < ((m_screen.x/50) * ViewHelper::getRatio())) {

            auto searchIt = zombie_map.find(it->getCurrentTarget());


            if(searchIt != zombie_map.end()) {
                zombie_map[searchIt->first] = zombie_map[searchIt->first] + 1;
            } else {
                zombie_map[it->getCurrentTarget()] = 1;
            }
            it++;
        } else {
            it++;
        }
     }

    //for each cluster in the zombie_map that has more than 2 members, add it to zombie_set to spawn a zombie
    for (const auto &pair : zombie_map) {
        if(pair.second >= 2) {
          zombie_set.insert(pair.first);
        }
    }

    //for each limb that has its centroid position in zombie_set, delete it from our vector of limbs
    for (auto it = limbs.begin(); it != limbs.end();) {
        bool is_contained = (zombie_set.find(it->getCurrentTarget()) != zombie_set.end());
        if(is_contained) {
            it->destroy();
            it = limbs.erase(it);
            if(it->getLimbType() == "leg") {
                m_legs_total--;
            } else {
                m_arms_total--;
            }
        } else {
            it++;
        }
    }

    return zombie_set;
}


void LimbsManager::destroy() {
    for (auto &limb : limbs)
        limb.destroy();
    /*for (auto &limb1 : collectedLegs_p1)
        limb1.destroy();
    for (auto &limb2 : collectedLegs_p2)
        limb2.destroy();*/

    limbs.clear();
    //collectedLegs_p1.clear();
    //collectedLegs_p2.clear();
}

bool operator<(vec2 const &a, vec2 const &b)
{
    return std::tie(a.x, a.y) < std::tie(b.x, b.y); //let std::tuple handle it for us
}