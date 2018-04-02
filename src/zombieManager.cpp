#include "zombieManager.hpp"

// initialize a zombie manager
bool ZombieManager::init(vec2 screen, const std::vector<vec2> &mapCollisionPoints) {
    m_mapCollisionPoints = mapCollisionPoints;
    m_screen = screen;
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
    return false;
}

//updating targets of zombies if necessary
void ZombieManager::check_targets(vec2 player1_pos, vec2 player2_pos) {

    for (auto &zombie : zombies) {
        if (getDistance(zombie.get_position(), player1_pos) > getDistance(zombie.get_position(), player2_pos)) {
            zombie.setCurrentTarget({static_cast<float>(player2_pos.x), static_cast<float>(player2_pos.y)});
        } else {
            zombie.setCurrentTarget({static_cast<float>(player1_pos.x), static_cast<float>(player1_pos.y)});
        }
    }
}

void ZombieManager::computeZPaths(float ms, const MapGrid &mapGrid) {
    if (zombies.size() <= 1) return;
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
            float step = 50 * (ms / 1000);
            vec2 dir;
            dir.x = nextNode.x * 100 - zombie.get_position().x;
            dir.y = nextNode.y * 100 - zombie.get_position().y;

            auto next_pos = scale(step, normalize(dir));

            zombie.move(next_pos);
            zombie.setLastPath(zombie.getCurrentPath());
            zombie.setLastTarget(target);
        }
    }
}


//check if players collide with any zombies
//make sure to apply timeout
int ZombieManager::check_collision_with_players(Player1 *p1, Player2 *p2, ToolboxManager *tb) {
    return 1;
}

void ZombieManager::destroy() {
    zombies.clear();
    m_mapCollisionPoints.clear();
}