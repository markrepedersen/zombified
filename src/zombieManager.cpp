#include "zombieManager.hpp"

// initialize a zombie manager
bool ZombieManager::init(vec2 screen, const std::vector<vec2> &mapCollisionPoints) {
    m_mapCollisionPoints = mapCollisionPoints;
    m_screen = screen;
    speed = 50;
    zombies.reserve(1000);
    zombiesInUse = false;
    return true;
}

// Renders the existing zombies
void ZombieManager::draw(const mat3 &projection) {
        for (auto &zombie : zombies) {
            zombie.draw(projection);
        }
}

unsigned long ZombieManager::getZombieCount() {
    return zombies.size();
}

void ZombieManager::transformZombies(std::vector<Renderable *> &container) {
    std::transform(zombies.begin(), zombies.end(), std::back_inserter(container), [](Zombie& entity) { return &entity; });
}

//spawn new zombie where clusters meet
bool ZombieManager::spawn_zombie(vec2 zombie_pos, vec2 player1_pos, vec2 player2_pos) {
    Zombie zombie;

//    if(!zombiesInUse) {
//        zombiesInUse = true;
        if (zombie.init()) {
            zombie.set_position(zombie_pos);

            if (getDistance(zombie_pos, player1_pos) > getDistance(zombie_pos, player2_pos)) {
                zombie.setCurrentTarget({static_cast<float>(player2_pos.x), static_cast<float>(player2_pos.y)});
                //std::cout << "current target is player 2" << std::endl;
            } else {
                zombie.setCurrentTarget({static_cast<float>(player1_pos.x), static_cast<float>(player1_pos.y)});
                //std::cout << "current target is player 1" << std::endl;
            }
            zombies.emplace_back(zombie);
            return true;
        }
//        zombiesInUse = false;
//    }
    return false;

}

//updating targets of zombies if necessary, as well as attack timeout time for each zombie if necessary
//if players are within reach of zombies, return the number of attacks for each player {player1damage, player2damage}
vec2 ZombieManager::update_zombies(float elapsed_ms, vec2 player1_pos, vec2 player2_pos) {
    float player1damage = 0.f;
    float player2damage = 0.f;

//    if(!zombiesInUse){
//        zombiesInUse = true;
        for (auto &zombie : zombies) {
            float distance_to_player1 = getDistance(zombie.get_position(), player1_pos);
            float distance_to_player2 = getDistance(zombie.get_position(), player2_pos);

            if(distance_to_player1 > distance_to_player2) {
                zombie.setCurrentTarget({static_cast<float>(player2_pos.x), static_cast<float>(player2_pos.y)});
            } else {
                zombie.setCurrentTarget({static_cast<float>(player1_pos.x), static_cast<float>(player1_pos.y)});
            }

            float currTimeout = zombie.getAttackTimeout();
            if (currTimeout > 0) {
                zombie.setAttackTimeout(currTimeout - elapsed_ms);
            } else {
                bool zombieAttacked = false;

                if(distance_to_player1 < (m_screen.x/30 * ViewHelper::getRatio())) {
                    player1damage++;
                    zombieAttacked = true;
                }

                if(distance_to_player2 < (m_screen.x/30 * ViewHelper::getRatio())) {
                    player2damage++;
                    zombieAttacked = true;
                }

                //if this zombie attacked something this round, set timeout for 3 seconds
                if (zombieAttacked) {
                    zombie.setAttackTimeout(1000);
                }
            }
        }
//        zombiesInUse = false;
//    }

    return {player1damage, player2damage};
}

void ZombieManager::computeZPaths(float ms, const MapGrid &mapGrid) {
    if (zombies.size() <= 1) return;
//    if(!zombiesInUse) {
//        zombiesInUse = true;
        for (auto &zombie : zombies) {
            JPS::PathVector path;
            vec2 target = zombie.getCurrentTarget();

            if (zombie.getLastTarget() != target || !zombie.isInitialized()) {
                auto srcX = (unsigned) (zombie.get_position().x / 100);
                auto srcY = (unsigned) (zombie.get_position().y / 100);
                auto dstX = (unsigned) (target.x / 100);
                auto dstY = (unsigned) (target.y / 100);
                JPS::findPath(path, mapGrid, srcX, srcY, dstX, dstY, 1);
                zombie.setCurrentPath(path);
                zombie.setInitialized(true);
            } else zombie.setCurrentPath(zombie.getLastPath());
            if (!zombie.getCurrentPath().empty()) {
                vec2 nextNode, curNode;
                curNode = nextNode = {zombie.get_position().x, zombie.get_position().y};

                for (int i = 0; i < zombie.getCurrentPath().size() && getDistance(curNode, nextNode) < 10; ++i) {
                    nextNode = {static_cast<float>(zombie.getCurrentPath()[i].x),
                        static_cast<float>(zombie.getCurrentPath()[i].y)};
                }
                float step = speed * (ms / 1000);
                vec2 dir;
                dir.x = nextNode.x * 300 - zombie.get_position().x;
                dir.y = nextNode.y * 300 - zombie.get_position().y;

                auto next_pos = scale(step, normalize(dir));

                zombie.move(next_pos);
                zombie.setLastPath(zombie.getCurrentPath());
                zombie.setLastTarget(target);
            }
        }
//        zombiesInUse = false;
//    }
}


//check if players collide with any zombies
//make sure to apply timeout
int ZombieManager::check_collision_with_players(Player1 *p1, Player2 *p2, ToolboxManager *tb) {
    return 1;
}

bool ZombieManager::attack_zombies(vec2 player_pos, vec2 player_boundingbox, int playerNum, ToolboxManager *m_toolboxmanager) {
//
//    if(!zombiesInUse) {
//        zombiesInUse = true;
        for (auto it = zombies.begin(); it != zombies.end();){
            if ((std::abs(player_pos.x - it->get_position().x) * 2 <
                 (player_boundingbox.x + it->get_bounding_box().x)) &&
                (std::abs(player_pos.y - it->get_position().y) * 2 <
                 (player_boundingbox.y + it->get_bounding_box().y)))
                    {
                        if(m_toolboxmanager->addSlot(playerNum))
                        {
                            it->destroy();
                            it = zombies.erase(it);
                            return true;
                        } else {
                            ++it;
                        }
                    }
                    else {
                        ++it;
                    }
        }
//        zombiesInUse = false;
//    }
    return false;
}

void ZombieManager::setSpeed(float speed) {
    this->speed = speed;
}
    
float ZombieManager::getSpeed() {
    return speed;
}

void ZombieManager::setZombiesInUse(bool inUse) {
    this->zombiesInUse = inUse;
}

bool ZombieManager::getZombiesInUse() {
    return zombiesInUse;
}


void ZombieManager::destroy() {
    for (auto &zombiescreated : zombies)
        zombiescreated.destroy();
    zombies.clear();
    m_mapCollisionPoints.clear();
}
