#include "world.hpp"
#include <sstream>
#include <unordered_set>

using namespace std;

namespace {
    const size_t MAX_ARMS = 8;
    const size_t MAX_LEGS = 3;
    const size_t MAX_FREEZE = 2;
    const size_t MAX_MISSILE = 2;
    const size_t MAX_ARMOUR = 2;
    const size_t MAX_BOMB = 1;
    const size_t MAX_WATER = 2;
    const size_t ARM_DELAY_MS = 1000;
    const size_t LEG_DELAY_MS = 1000;
    const size_t DELAY_MS = 1000;

    namespace {
        void glfw_err_cb(int error, const char *desc) {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}


World::World() :
        m_next_arm_spawn(rand() % (1000) + 500),
        m_next_leg_spawn(rand() % (1000) + 500),
        m_next_spawn(rand() % (1000) + 600) {
    m_rng = std::default_random_engine(std::random_device()());
}

World::~World() {
}

bool World::init(vec2 screen) {
    
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 0);
    m_window = glfwCreateWindow((int) screen.x, (int) screen.y, "Zombified", nullptr, nullptr);
    if (m_window == nullptr)
        return false;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync
    gl3wInit();
    glfwSetWindowUserPointer(m_window, this);
    auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) {
        ((World *) glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3);
    };
    auto mouse_button_callback = [](GLFWwindow *wnd, int _0, int _1, int _2) {
        ((World *) glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1, _2);
    };

    glfwSetKeyCallback(m_window, key_redirect);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);

    ViewHelper::getInstance(m_window);
    populateMapCollisionPoints();
    mapGrid = new MapGrid((unsigned) screen.x / 100, (unsigned) screen.y / 100);
    //m_limbsManager.init(screen, mapCollisionPoints);

    bool rendered = false;
    game_started = false;
    game_over = false;
    game_over_limbo = false;
    instruction_page = false;
    currWinnerName = "";
    infoscreen = "none";
    pause = false;
    m_min = 0;
    m_sec = 0;
    
    rendered = (m_infobutton.init("info")&&
                m_startbutton.init("start") &&
                m_backbutton.init("back")&&
                key_info.init("key") &&
                story_info.init("story") &&
                m_pause.init("pause") &&
                m_infopage.init("tool") &&
                m_freezedetails.init("freeze") &&
                m_waterdetails.init("water") &&
                m_muddetails.init("mud") &&
                m_bombdetails.init("bomb") &&
                m_missiledetails.init("missile") &&
                m_armourdetails.init("armour") &&
                m_winner1.init("winner1")&&
                m_winner2.init("winner2"));

    
    return rendered;
}

void World::destroy() {
    
    m_worldtexture.destroy();
    m_toolboxManager.destroy();
    m_player1.destroy();
    m_player2.destroy();
    m_limbsManager.destroy();
    m_zombieManager.destroy();
    for (auto &freeze : m_freeze)
        freeze.destroy();
    for (auto &water : m_water)
        water.destroy();
    for (auto &missile : m_missile)
        missile.destroy();
    for (auto &armour : m_armour)
        armour.destroy();
    for (auto &bomb : m_bomb)
        bomb.destroy();
    for (auto &explosion : m_explosion)
        explosion.destroy();
    for (auto &freeze_collected : m_freeze_collected_1)
        freeze_collected.destroy();
    for (auto &water_collected : m_water_collected_1)
        water_collected.destroy();
    for (auto &freeze_collected : m_freeze_collected_2)
        freeze_collected.destroy();
    for (auto &water_collected : m_water_collected_2)
        water_collected.destroy();

    for (auto &missile_collected : m_missile_collected_1)
        missile_collected.destroy();
    for (auto &missile_collected : m_missile_collected_2)
        missile_collected.destroy();
    for (auto &bomb_collected : m_bomb_collected_1)
        bomb_collected.destroy();
    for (auto &bomb_collected : m_bomb_collected_2)
        bomb_collected.destroy();
    for (auto &armour_collected : m_armour_collected_1)
        armour_collected.destroy();
    for (auto &armour_collected : m_armour_collected_2)
        armour_collected.destroy();
    for (auto &mud_collected : m_mud_collected)
        mud_collected.destroy();


    for (auto &bomb_used : used_bombs)
        bomb_used.destroy();
    for (auto &missiles_used : used_missiles)
        missiles_used.destroy();

    m_freeze.clear();
    m_water.clear();
    m_missile.clear();
    m_armour.clear();
    m_bomb.clear();
    m_explosion.clear();
    m_freeze_collected_1.clear();
    m_freeze_collected_2.clear();
    m_water_collected_1.clear();
    m_water_collected_2.clear();
    m_missile_collected_1.clear();
    m_missile_collected_2.clear();
    m_bomb_collected_1.clear();
    m_bomb_collected_2.clear();
    m_armour_collected_1.clear();
    m_armour_collected_2.clear();
    used_bombs.clear();
    used_missiles.clear();
    m_mud_collected.clear();
    
    //mapCollisionPoints.clear();

}

bool World::update(float elapsed_ms) {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    vec2 screen = {(float) w, (float) h};
    
    if (game_over) {
        /*if (winner == 1)
            m_winner1.init("winner1");
        else if (winner == 2)
            m_winner2.init("winner2");*/
        game_over = false;
        if(winner != 0) {
            game_over_limbo = true;
        }
        game_started = false;
        return true;
        
    }
    
    else if (!game_started) {
        
        if (!instruction_page){
            if (m_infobutton.is_clicked()) {
                instruction_page = true;
                m_infobutton.unclick();
                return true;
            }
            
            else if (m_startbutton.is_clicked() && !game_over_limbo) {
                
                gl_has_errors();
                game_started = true;
                m_startbutton.unclick();

                pause = false;
                
                /*if (winner == 1)
                    m_winner1.destroy();
                else if (winner == 2)
                    m_winner2.destroy();
                winner = 0;*/
                
                //populateMapCollisionPoints();
                
                bool initialized = (gloveRight_p1.init(screen)&&
                                    gloveLeft_p1.init(screen)&&
                                    
                                    gloveRight_p2.init(screen)&&
                                    gloveLeft_p2.init(screen)&&
                                    
                                    m_worldtexture.init(screen)&&
                                    m_toolboxManager.init({screen.x, screen.y}) &&
                                    m_antidote.init(screen, mapCollisionPoints) &&
                                    m_player1.init(screen, mapCollisionPoints) &&
                                    m_player2.init(screen, mapCollisionPoints) &&
                                    
                                    m_zombieManager.init({screen.x, screen.y}, mapCollisionPoints) &&
                                    m_limbsManager.init(screen, mapCollisionPoints));
                
                srand((unsigned) time(0));
                explosion = false;
                m_min = 1;
                m_sec = 0;
                timeDelay = 5;
                start = time(0);
                immobilize = 0;
                armourInUse_p1 = false;
                armourInUse_p2 = false;
                droptool_p1 = false;
                droptool_p2 = false;
                useBomb = false;
                useMissile = false;
                is_punchingright_p1 = false;
                is_punchingleft_p1 = false;
                is_punchingright_p2 = false;
                is_punchingleft_p2 = false;
                
                droppedAntidoteTime_p1 = time(0);
                droppedAntidoteTime_p2 = time(0);
                
                m_limbsManager.set_arms_size(0);
                m_limbsManager.set_legs_size(0);
                
                m_player1.set_key(0, false);
                m_player1.set_key(1, false);
                m_player1.set_key(2, false);
                m_player1.set_key(3, false);
                m_player2.set_key(0, false);
                m_player2.set_key(1, false);
                m_player2.set_key(2, false);
                m_player2.set_key(3, false);
                
                return initialized;
                
            }
            
        }
    }

    else if (game_started) {
        if ((int) difftime(time(0), start) == timeDelay)
            timer_update();
        if (!game_over){
            if (!pause) {
                m_player1.update(elapsed_ms);
                m_player2.update(elapsed_ms);
                
                random_spawn(elapsed_ms, {screen.x * ViewHelper::getRatio(), screen.y * ViewHelper::getRatio()});
                vec2 player_hits_from_zombies = m_zombieManager.update_zombies(elapsed_ms, m_player1.get_position(), m_player2.get_position());
                
                if(!armourInUse_p1) {m_player1.numberofHits += player_hits_from_zombies.x;}
                if(!armourInUse_p2) {m_player2.numberofHits += player_hits_from_zombies.y;}
                
                m_limbsManager.computePaths(elapsed_ms, *mapGrid);
                //if the freeze item is used, then zombies will stop moving
                if ((int) difftime(time(0), freezeTime) >= 5)
                {
                    m_zombieManager.computeZPaths(elapsed_ms, *mapGrid);
                }
                std::unordered_set<vec2> new_zombie_positions = m_limbsManager.checkClusters();
                if (!new_zombie_positions.empty()) {
                    for (const vec2 &pos : new_zombie_positions) {
                        //std::cout << "new zombie!!" << pos.x << ", " << pos.y << std::endl;
                        m_zombieManager.spawn_zombie(pos, m_player1.get_position(), m_player2.get_position());
                    }
                }
                
                check_add_tools(screen);
                
                if (explosion)
                    explode();
                
                if (useBomb)
                    use_bomb(elapsed_ms);
                
                if (useMissile)
                    use_missile(elapsed_ms);
                
                if (armourInUse_p1){
                    if ((int) difftime(time(0), armourTime_p1) >= 10) {
                        armourInUse_p1 = false;
                        m_player1.set_armourstate(false);
                        armourTime_p1 = 0;
                    }
                }
                if (armourInUse_p2) {
                    if ((int) difftime(time(0), armourTime_p2) >= 10) {
                        armourInUse_p2 = false;
                        m_player2.set_armourstate(false);
                        armourTime_p2 = 0;
                    }
                }
                // check how many times the player has been hit
                // if player was hit 5 times, drops items
                if (m_player1.numberofHits >= 5) {
                    droptool_p1 = true;
                    use_tool_1(m_toolboxManager.useItem(1));
                    m_player1.numberofHits = 0;
                }
                if (m_player2.numberofHits >= 5) {
                    droptool_p2 = true;
                    use_tool_2(m_toolboxManager.useItem(2));
                    m_player2.numberofHits = 0;
                }
                
                if (m_limbsManager.getCollectedLegs(1) > 0){
                    if ((int) difftime(time(0), leg_times_1) >= 10){
                        //fprintf(stderr, "remove leg1 \n");
                        m_limbsManager.decreaseCollectedLegs(1);
                        m_player2.increase_speed_legs(-10);
                        leg_times_1 = time(0);
                    }
                }
                
                if (m_limbsManager.getCollectedLegs(2) > 0){
                    if ((int) difftime(time(0), leg_times_2) >= 10){
                        //fprintf(stderr, "remove leg2 \n");
                        m_limbsManager.decreaseCollectedLegs(2);
                        m_player2.increase_speed_legs(-10);
                        leg_times_2 = time(0);
                    }
                }
                
                if(is_punchingleft_p2)
                    gloveLeft_p2.set_position({m_player2.get_position().x-(30.f * ViewHelper::getRatio()),
                        m_player2.get_position().y+(15.f * ViewHelper::getRatio())});
                if(is_punchingright_p2)
                    gloveRight_p2.set_position({m_player2.get_position().x+(30.f * ViewHelper::getRatio()),
                        m_player2.get_position().y+(15.f * ViewHelper::getRatio())});
                if(is_punchingleft_p1)
                    gloveLeft_p1.set_position({m_player1.get_position().x- (30.f * ViewHelper::getRatio()),
                        m_player1.get_position().y+ (15.f * ViewHelper::getRatio())});
                if(is_punchingright_p1)
                    gloveRight_p1.set_position({m_player1.get_position().x+ (30.f * ViewHelper::getRatio()),
                        m_player1.get_position().y+ (15.f * ViewHelper::getRatio())});
            }
        }
    }
    return true;
}

void World::timer_update() {
    if (!pause) {
        if (m_min == 0 && m_sec == 0) {
            m_min = 0;
            m_sec = 0;
            fprintf(stderr, "winner is %d \n", m_antidote.belongs_to);
            winner = m_antidote.belongs_to;
            destroy();
            
            game_over = true;
            
        } else if (m_sec == 0) {
            m_sec = 59;
            m_min -= 1;
            timeDelay++;
        } else {
            m_sec -= 1;
            timeDelay++;
        }
    }
    else
        timeDelay++;

}

// Render our game world
void World::draw() {
    gl_flush_errors();

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    std::stringstream title_ss;
    if (m_sec < 10)
        title_ss << "player1 numberoflegs: " << m_limbsManager.getCollectedLegs(1) << "          "
        << "player1 damage count: " << m_player1.numberofHits << "          "
        << "Time remaining " << m_min << ":" << "0" << m_sec << "          "
        << "player2 damage count: " << m_player2.numberofHits << "          "
        << "player2 numberoflegs: " << m_limbsManager.getCollectedLegs(2);
    else
        title_ss << "player1 numberoflegs: " << m_limbsManager.getCollectedLegs(1) << "          "
        << "player1 damage count: " << m_player1.numberofHits << "          "
        << "Time remaining " << m_min << ":" << m_sec << "          "
        << "player2 damage count: " << m_player2.numberofHits << "          "
        << "player2 numberoflegs: " << m_limbsManager.getCollectedLegs(2);
    glfwSetWindowTitle(m_window, title_ss.str().c_str());

    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);
   const float clear_color[3] = {0.1176f, 0.1451f, 0.1725f};
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float left = 0.f;        // *-0.5;
    float top = 0.f;         // (float)h * -0.5;
    float right = (float) w;  // *0.5;
    float bottom = (float) h; // *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);

    mat3 projection_2D{{sx,  0.f, 0.f},
                       {0.f, sy,  0.f},
                       {tx,  ty,  1.f}};

    if (!game_started) {
        
        if (instruction_page){
            instructionScreenDraw(projection_2D);
            return;
        }
        
        else if (!instruction_page){
            startScreenDraw(projection_2D);
            return;
        }

    }
    else {
            
        //these should always be drawn first
        m_worldtexture.draw(projection_2D);
        m_toolboxManager.draw(projection_2D);

        //these are drawn in ascending order w.r.t. their y position
        m_limbsManager.draw(projection_2D);
        m_zombieManager.draw(projection_2D);
        
        for (auto& mud_collected: m_mud_collected)
            mud_collected.draw(projection_2D);
        
        entityDrawOrder(projection_2D);
        
        if (is_punchingleft_p1)
            gloveLeft_p1.draw(projection_2D);
        if (is_punchingright_p1)
            gloveRight_p1.draw(projection_2D);
        
        if (is_punchingleft_p2)
            gloveLeft_p2.draw(projection_2D);
        if (is_punchingright_p2)
            gloveRight_p2.draw(projection_2D);
        
        if (pause)
            m_pause.draw(projection_2D);
        
        glfwSwapBuffers(m_window);
        return;
    }
}

void World::instructionScreenDraw(mat3 projection_2D) {
    m_backbutton.draw(projection_2D);
    m_infopage.draw(projection_2D);
    
    if (infoscreen == "freeze")
        m_freezedetails.draw(projection_2D);
    else if (infoscreen == "water")
        m_waterdetails.draw(projection_2D);
    else if (infoscreen == "mud")
        m_muddetails.draw(projection_2D);
    else if (infoscreen == "bomb")
        m_bombdetails.draw(projection_2D);
    else if (infoscreen == "missile")
        m_missiledetails.draw(projection_2D);
    else if (infoscreen == "armour")
        m_armourdetails.draw(projection_2D);
    
    glfwSwapBuffers(m_window);
}

void World::startScreenDraw(mat3 projection_2D) {
    m_startbutton.draw(projection_2D);
    m_infobutton.draw(projection_2D);
    key_info.draw(projection_2D);
    story_info.draw(projection_2D);
    
    if (winner == 1)
        m_winner1.draw(projection_2D);
    else if (winner == 2)
        m_winner2.draw(projection_2D);
    
    glfwSwapBuffers(m_window);
}

void World::entityDrawOrder(mat3 projection_2D) {
    vector<Renderable *> drawOrderVector;

    drawOrderVector.reserve(
            m_freeze.size() +
            m_water.size() +
            m_missile.size() +
            m_armour.size() +
            used_missiles.size() +
            m_bomb.size() +
            used_bombs.size() +
            m_explosion.size() +
            m_water_collected_1.size() +
            m_water_collected_2.size() +
            m_freeze_collected_1.size() +
            m_freeze_collected_2.size() +
            m_missile_collected_1.size() +
            m_missile_collected_2.size() +
            m_bomb_collected_1.size() +
            m_bomb_collected_2.size() +
            m_armour_collected_1.size() +
            m_armour_collected_2.size() +
            //m_mud_collected.size() +
            m_zombieManager.getZombieCount() +
            m_limbsManager.getLimbCount()
            + 3);

    transform(m_freeze.begin(), m_freeze.end(), std::back_inserter(drawOrderVector),
              [](Ice& entity) { return &entity; });
    transform(m_water.begin(), m_water.end(), std::back_inserter(drawOrderVector),
              [](Water& entity) { return &entity; });
    transform(m_missile.begin(), m_missile.end(), std::back_inserter(drawOrderVector),
              [](Missile& entity) { return &entity; });
    transform(used_missiles.begin(), used_missiles.end(), std::back_inserter(drawOrderVector),
              [](Missile& entity) { return &entity; });
    transform(m_armour.begin(), m_armour.end(), std::back_inserter(drawOrderVector),
              [](Armour& entity) { return &entity; });
    transform(m_bomb.begin(), m_bomb.end(), std::back_inserter(drawOrderVector),
                [](Bomb& entity) { return &entity; });
    transform(used_bombs.begin(), used_bombs.end(), std::back_inserter(drawOrderVector),
              [](Bomb& entity) { return &entity; });
    transform(m_explosion.begin(), m_explosion.end(), std::back_inserter(drawOrderVector),
              [](Explosion& entity) { return &entity; });
    transform(m_water_collected_1.begin(), m_water_collected_1.end(), std::back_inserter(drawOrderVector),
              [](Water& entity) { return &entity; });
    transform(m_water_collected_2.begin(), m_water_collected_2.end(), std::back_inserter(drawOrderVector),
              [](Water& entity) { return &entity; });
    transform(m_freeze_collected_1.begin(), m_freeze_collected_1.end(), std::back_inserter(drawOrderVector),
              [](Ice& entity) { return &entity; });
    transform(m_freeze_collected_2.begin(), m_freeze_collected_2.end(), std::back_inserter(drawOrderVector),
              [](Ice& entity) { return &entity; });
    transform(m_missile_collected_1.begin(), m_missile_collected_1.end(), std::back_inserter(drawOrderVector),
              [](Missile& entity) { return &entity; });
    transform(m_missile_collected_2.begin(), m_missile_collected_2.end(), std::back_inserter(drawOrderVector),
              [](Missile& entity) { return &entity; });
    transform(m_bomb_collected_1.begin(), m_bomb_collected_1.end(), std::back_inserter(drawOrderVector),
              [](Bomb& entity) { return &entity; });
    transform(m_bomb_collected_2.begin(), m_bomb_collected_2.end(), std::back_inserter(drawOrderVector),
              [](Bomb& entity) { return &entity; });
    transform(m_armour_collected_1.begin(), m_armour_collected_1.end(), std::back_inserter(drawOrderVector),
              [](Armour& entity) { return &entity; });
    transform(m_armour_collected_2.begin(), m_armour_collected_2.end(), std::back_inserter(drawOrderVector),
              [](Armour& entity) { return &entity; });
    //transform(m_mud_collected.begin(), m_mud_collected.end(), std::back_inserter(drawOrderVector),
    //          [](Mud& entity) { return &entity; });

    drawOrderVector.push_back(&m_player2);
    drawOrderVector.push_back(&m_player1);
    drawOrderVector.push_back(&m_antidote);

    m_zombieManager.transformZombies(drawOrderVector);
    m_limbsManager.transformLimbs(drawOrderVector);

    sort(drawOrderVector.begin(), drawOrderVector.end(), [](Renderable *lhs, Renderable *rhs) {
        return lhs->m_position.y < rhs->m_position.y;
    });

    for_each(drawOrderVector.begin(), drawOrderVector.end(), [projection_2D](Renderable *entity) {
        entity->draw(projection_2D);
    });
}


// Should the game be over ?
bool World::is_over() const {
    return glfwWindowShouldClose(m_window);
}

// On key callback
void World::on_key(GLFWwindow *, int key, int, int action, int mod) {
    
    if (action == GLFW_PRESS && key == GLFW_KEY_P){
        if (pause)
            pause = false;
        else if (!pause)
            pause = true;
    }
    
    if(game_over_limbo) {
        std::cout<< "in limbo" << std::endl;
        int a = 'A';
        int z = 'Z';
        int a2 = 'a';
        int z2 = 'z';

        if (action == GLFW_RELEASE)
        {
            string temp;
            //if key is a letter key
            if((key > a && key < z) || (key > a2 && key < z2)){
                temp =(char) key;
                std::cout<< temp << std::endl;
                currWinnerName.append(temp);
                std::cout << currWinnerName << std::endl;
            } else if (key == GLFW_KEY_ENTER) {
                std::cout<< "saving now" << currWinnerName << std::endl;
                saveToFile(currWinnerName);
                game_over_limbo = false;
                currWinnerName = "";
            } else if (key == GLFW_KEY_BACKSPACE) {
                currWinnerName = currWinnerName.substr(0, currWinnerName.length() - 1);
            }
        }
        //get keys
        //if enter key pressed,
        //save to file and game_over_limbo == false
    }
    else if (!pause){
        // player2 actions
        if (immobilize != 2 && !m_player2.get_blowback()) {
            if (action == GLFW_PRESS && key == GLFW_KEY_UP)
                m_player2.set_key(0, true);
            if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
                m_player2.set_key(1, true);
            if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
                m_player2.set_key(2, true);
            if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
                m_player2.set_key(3, true);
            if (action == GLFW_RELEASE && key == GLFW_KEY_UP)
                m_player2.set_key(0, false);
            if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT)
                m_player2.set_key(1, false);
            if (action == GLFW_RELEASE && key == GLFW_KEY_DOWN)
                m_player2.set_key(2, false);
            if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT)
                m_player2.set_key(3, false);
            
            if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT_SHIFT) {
                
                bool hasTools = false;
                for (auto &tools : m_toolboxManager.getListOfSlot_2()) {
                    if (tools != 0 && tools != 3) {
                        hasTools = true;
                        break;
                    }
                }
                
                if(m_zombieManager.attack_zombies(m_player2.get_position(), m_player2.get_bounding_box(), 2, &m_toolboxManager))
                {
                    if((m_player1.lastkey == 2) || (m_player1.lastkey == 1)) {
                        is_punchingleft_p2 = true;
                    } else {
                        is_punchingright_p2= true;
                    }
                }else {
                    if (immobilize == 1){
                        //fprintf(stderr, "player2 number of hits: %d \n", m_player2.numberofHits);
                        if ((m_player2.lastkey == 2) || (m_player2.lastkey == 1)) { //up and left
                            //fprintf(stderr, "left\n");
                            is_punchingleft_p2 = true;
                            gloveLeft_p2.set_position({m_player2.get_position().x-(30.f * ViewHelper::getRatio()),
                                m_player2.get_position().y+(15.f * ViewHelper::getRatio())});
                            if (m_player1.collides_with(gloveLeft_p2) && !armourInUse_p1)
                                m_player1.numberofHits++;
                        }
                        else if ((m_player2.lastkey == 0) || (m_player2.lastkey == 3)) { //down and right
                            //fprintf(stderr, "right\n");
                            is_punchingright_p2 = true;
                            gloveRight_p2.set_position({m_player2.get_position().x+(30.f * ViewHelper::getRatio()),
                                m_player2.get_position().y+(15.f * ViewHelper::getRatio())});
                            if (m_player1.collides_with(gloveRight_p2) && !armourInUse_p1)
                                m_player1.numberofHits++;
                        }
                        
                        //m_player1.numberofHits++;
                    }
                    else {
                        
                        // if the player has no tools then can manually attack, or else, just use a tool
                        if (!hasTools) {
                            if ((m_player2.lastkey == 2) || (m_player2.lastkey == 1)) { //up and left
                                //fprintf(stderr, "left\n");
                                is_punchingleft_p2 = true;
                                gloveLeft_p2.set_position(
                                                          {m_player2.get_position().x - (30.f * ViewHelper::getRatio()),
                                                              m_player2.get_position().y + (15.f * ViewHelper::getRatio())});
                                if (m_player1.collides_with(gloveLeft_p2) && !armourInUse_p1)
                                    m_player1.numberofHits++;
                            } else if ((m_player2.lastkey == 0) || (m_player2.lastkey == 3)) { //down and right
                                //fprintf(stderr, "right\n");
                                is_punchingright_p2 = true;
                                gloveRight_p2.set_position(
                                                           {m_player2.get_position().x + (30.f * ViewHelper::getRatio()),
                                                               m_player2.get_position().y + (15.f * ViewHelper::getRatio())});
                                if (m_player1.collides_with(gloveRight_p2) && !armourInUse_p1)
                                    m_player1.numberofHits++;
                            }
                        } else
                            use_tool_2(m_toolboxManager.useItem(2));
                        //fprintf(stderr, "player2 number of hits: %d \n", m_player2.numberofHits);
                    }
                }
                
               
                //fprintf(stderr, "player2 number of hits: %d \n", m_player2.numberofHits);
            }
            if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT_SHIFT) {
                is_punchingleft_p2 = false;
                is_punchingright_p2 = false;
            }
        }
        if (immobilize == 2 || m_player2.get_blowback()) //player is frozen
        {
            //m_player2.set_freezestate(true);
            //fprintf(stderr, "frozen");
            m_player2.set_key(0, false);
            m_player2.set_key(1, false);
            m_player2.set_key(2, false);
            m_player2.set_key(3, false);
            if ((int) difftime(time(0), freezeTime) >= 5) {
                immobilize = 0;
                m_player2.set_freezestate(false);
                freezeTime = 0;
                //fprintf(stderr, "start");
            }
        }
        
        // player1 actions
        if (immobilize != 1 && !m_player1.get_blowback()) {
            if (action == GLFW_PRESS && key == GLFW_KEY_W)
                m_player1.set_key(0, true);
            if (action == GLFW_PRESS && key == GLFW_KEY_A)
                m_player1.set_key(1, true);
            if (action == GLFW_PRESS && key == GLFW_KEY_S)
                m_player1.set_key(2, true);
            if (action == GLFW_PRESS && key == GLFW_KEY_D)
                m_player1.set_key(3, true);
            if (action == GLFW_RELEASE && key == GLFW_KEY_W)
                m_player1.set_key(0, false);
            if (action == GLFW_RELEASE && key == GLFW_KEY_A)
                m_player1.set_key(1, false);
            if (action == GLFW_RELEASE && key == GLFW_KEY_S)
                m_player1.set_key(2, false);
            if (action == GLFW_RELEASE && key == GLFW_KEY_D)
                m_player1.set_key(3, false);
            
            // use tools
            if (action == GLFW_PRESS && key == GLFW_KEY_Q) {
                // if the player has no tools/no slots attack the other player/zombie by colliding and pressing attack
                bool hasTools = false;
                for (auto &tools : m_toolboxManager.getListOfSlot_1()) {
                    if (tools != 0 && tools != 3) {
                        hasTools = true;
                        break;
                    }
                }
                if(m_zombieManager.attack_zombies(m_player1.get_position(), m_player1.get_bounding_box(), 1, &m_toolboxManager))
                {
                    if((m_player1.lastkey == 2) || (m_player1.lastkey == 1)) {
                        is_punchingleft_p1 = true;
                    } else {
                        is_punchingright_p1 = true;
                    }
                } else {
                    //if (m_player1.collides_with(m_player2)) {
                    if (immobilize == 2)
                    {
                        if ((m_player1.lastkey == 2) || (m_player1.lastkey == 1)) { //up and left
                            //fprintf(stderr, "left p1\n");
                            is_punchingleft_p1 = true;
                            gloveLeft_p1.set_position({m_player1.get_position().x-(30.f * ViewHelper::getRatio()),
                                m_player1.get_position().y+(15.f* ViewHelper::getRatio())});
                            if (m_player2.collides_with(gloveLeft_p1) && !armourInUse_p2)
                                m_player2.numberofHits++;
                        }
                        else if ((m_player1.lastkey == 0) || (m_player1.lastkey == 3)) { //down and right
                            //fprintf(stderr, "right\n");
                            is_punchingright_p1 = true;
                            gloveRight_p1.set_position({m_player1.get_position().x+(30.f * ViewHelper::getRatio()),
                                m_player1.get_position().y+(15.f * ViewHelper::getRatio())});
                            if (m_player2.collides_with(gloveRight_p1) && !armourInUse_p2)
                                m_player2.numberofHits++;
                        }
                    }
                    else
                    {
                        
                        if (!hasTools) {
                            //if (immobilize == 2) {
                            if ((m_player1.lastkey == 2) || (m_player1.lastkey == 1)) { //up and left
                                //fprintf(stderr, "left\n");
                                is_punchingleft_p1 = true;
                                gloveLeft_p1.set_position({m_player1.get_position().x-(30.f * ViewHelper::getRatio()),
                                    m_player1.get_position().y+(15.f * ViewHelper::getRatio())});
                                if (m_player2.collides_with(gloveLeft_p1) && !armourInUse_p2)
                                    m_player2.numberofHits++;
                            }
                            else if ((m_player1.lastkey == 0) || (m_player1.lastkey == 3)) { //down and right
                                //fprintf(stderr, "right\n");
                                is_punchingright_p1 = true;
                                gloveRight_p1.set_position({m_player1.get_position().x+(30.f * ViewHelper::getRatio()),
                                    m_player1.get_position().y+(15.f * ViewHelper::getRatio())});
                                if (m_player2.collides_with(gloveRight_p1) && !armourInUse_p2)
                                    m_player2.numberofHits++;
                            }
                            
                        }
                        else
                            use_tool_1(m_toolboxManager.useItem(1));
                    }
                }
                
                
            }
            if (action == GLFW_RELEASE && key == GLFW_KEY_Q) {
                is_punchingright_p1 = false;
                is_punchingleft_p1 = false;
            }
        }
        if (immobilize == 1 || m_player1.get_blowback()) //player is frozen
        {
            //m_player1.set_freezestate(true);
            m_player1.set_key(0, false);
            m_player1.set_key(1, false);
            m_player1.set_key(2, false);
            m_player1.set_key(3, false);
            if ((int) difftime(time(0), freezeTime) >= 5) {
                m_player1.set_freezestate(false);
                immobilize = 0;
                freezeTime = 0;
            }
        }
    }
}

void World::on_mouse_move(GLFWwindow *window, int button, int action, int mod) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    //std::cout << "xpos: " << xpos << std::endl;
    //std::cout << "ypos: " << ypos << std::endl;
    
    if (!game_started) {
        if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
            //std::cout << "xpos: " << xpos << std::endl;
            //std::cout << "ypos: " << ypos << std::endl;
            if (instruction_page){
                if (xpos < 480.f && xpos > 340.f && ypos < 215.f && ypos > 90.f) {
                    //std::cout << "ice" << std::endl;
                    infoscreen = "freeze";
                }
                else if (xpos < 490.f && xpos > 345.f && ypos < 445.f && ypos > 290.f) {
                    //std::cout << "water" << std::endl;
                    infoscreen = "water";
                }
                else if (xpos < 615.f && xpos > 260.f && ypos < 615.f && ypos > 540.f) {
                    //std::cout << "mud" << std::endl;
                    infoscreen = "mud";
                }
                else if (xpos < 1010.f && xpos > 875.f && ypos < 215.f && ypos > 85.f) {
                    //std::cout << "bomb" << std::endl;
                    infoscreen = "bomb";
                }
                else if (xpos < 995.f && xpos > 870.f && ypos < 450.f && ypos > 290.f) {
                    //std::cout << "missile" << std::endl;
                    infoscreen = "missile";
                }
                else if (xpos < 1020.f && xpos > 855.f && ypos < 640.f && ypos > 525.f) {
                    //std::cout << "armour" << std::endl;
                    infoscreen = "armour";
                }
                else if (xpos < 175.f && xpos > 20.f && ypos < 80.f && ypos > 22.f) {
                    //std::cout << "back" << std::endl;
                    instruction_page = false;
                }
                
                //draw();
            }
            else {
                if (xpos < 780.f && xpos > 505.f && ypos < 345.f && ypos > 280.f)
                    m_startbutton.clickicon();
            }
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            //std::cout << "xpos: " << xpos << std::endl;
            //std::cout << "ypos: " << ypos << std::endl;
            if (!instruction_page && !game_over_limbo){
                //click start button
                //std::cout << "xpos: " << xpos << std::endl;
                //std::cout << "ypos: " << ypos << std::endl;
                if (xpos < 780.f && xpos > 505.f && ypos < 345.f && ypos > 280.f){
                    m_startbutton.click();
                    draw();
                }
                
                //click info button
                if (xpos < 1300.f && xpos > 970.f && ypos < 700.f && ypos > 630.f)
                {
                    //std::cout << "info page!" << std::endl;
                    infoscreen = "default";
                    m_infobutton.click();
                }
                //draw();
                
            }
            
        }
    }
    else {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {

             std::cout << "mapCollisionPoints.push_back({ " << xpos << "f * ViewHelper::getRatio(), " << ypos << "f * ViewHelper::getRatio()});" << std::endl;
//            std::cout << "xpos: " << xpos << std::endl;
//            std::cout << "ypos: " << ypos << std::endl;
//            std::cout << "player pos: " << m_player1.get_position().x << ", " << m_player1.get_position().y
//                      << std::endl;
//            if (isInsidePolygon(mapCollisionPoints, {(float)xpos * ViewHelper::getRatio(), (float)ypos * ViewHelper::getRatio()})) {
//                std::cout << "yes it's inside polygon" << std::endl;
//            } else {
//                std::cout << "nope, it's outside the polygon" << std::endl;
//            }
        }
    }
}

bool World::spawn_freeze() {
    Ice freeze;
    if (freeze.init()) {
        m_freeze.emplace_back(freeze);
        return true;
    }
    return false;
}

bool World::spawn_missile() {
    Missile missile;
    if (missile.init()) {
        m_missile.emplace_back(missile);
        return true;
    }
    return false;
}

bool World::spawn_armour() {
    Armour armour;
    if (armour.init()) {
        m_armour.emplace_back(armour);
        return true;
    }
    return false;
}

bool World::spawn_bomb() {
    Bomb bomb;
    if (bomb.init()) {
        m_bomb.emplace_back(bomb);
        return true;
    }
    return false;
}

bool World::create_explosion(vec2 bomb_position) {
    Explosion explosion;
    if (explosion.init(bomb_position)) {
        m_explosion.emplace_back(explosion);
        return true;
    }
    return false;
}

bool World::spawn_water() {
    Water water;
    if (water.init()) {
        m_water.emplace_back(water);
        return true;
    }
    return false;
}

bool World::random_spawn(float elapsed_ms, vec2 screen) {
    m_next_arm_spawn -= elapsed_ms;
    m_next_leg_spawn -= elapsed_ms;
    m_next_spawn -= elapsed_ms;

    //srand((unsigned) time(0));
    int randNum = (rand() % (10)) + 1;

    //if (randNum % 2 == 0 || randNum % 1 == 0) {
    if (randNum == 1 || randNum == 10 || randNum == 8 || m_limbsManager.get_arms_size() == 0){
        if (m_limbsManager.get_arms_size() <= MAX_ARMS && m_next_arm_spawn < 0.f) {
            if (!(m_limbsManager.spawn_arms()))
                return false;
            m_next_arm_spawn = (ARM_DELAY_MS / 2) + rand() % (1000);
        }
    } else if (randNum == 2) {
        if (m_limbsManager.get_legs_size() <= MAX_LEGS && m_next_leg_spawn < 0.f) {
            if (!(m_limbsManager.spawn_legs()))
                return false;
            m_next_leg_spawn = (LEG_DELAY_MS / 2) + rand() % (1000);
        }
    } else if (randNum == 7) {
        if (m_freeze.size() <= MAX_FREEZE && m_next_spawn < 0.f) {
            if (!spawn_freeze())
                return false;
            Ice &new_freeze = m_freeze.back();
            new_freeze.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    } else if (randNum == 4) {
        if (m_missile.size() <= MAX_MISSILE && m_next_spawn < 0.f) {
            if (!spawn_missile())
                return false;

            Missile &new_missile = m_missile.back();
            // new_missile.set_position({(float) ((rand() % (int) screen.x)),
            //                           (float) ((rand() % (int) screen.y))});


            new_missile.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    } else if (randNum == 5) {
        if (m_armour.size() <= MAX_ARMOUR && m_next_spawn < 0.f) {
            if (!spawn_armour())
                return false;

            Armour &new_armour = m_armour.back();
            // new_armour.set_position({(float)((rand() % (int)screen.x)),
            //     (float)((rand() % (int)screen.y))});


            new_armour.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    } else if (randNum == 6 || randNum == 9) {
        if (m_bomb.size() <= MAX_BOMB && m_next_spawn < 0.f) {
            if (!spawn_bomb())
                return false;

            Bomb &new_bomb = m_bomb.back();
            // new_bomb.set_position({(float)((rand() % (int)screen.x)),
            //     (float)((rand() % (int)screen.y))});

            new_bomb.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    } else if (randNum == 7) {
        if (m_water.size() <= MAX_WATER && m_next_spawn < 0.f) {
            if (!spawn_water())
                return false;
            Water &new_water = m_water.back();
            new_water.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }
    return true;
}

// =========== CHECK IF NEED TO ADD TOOLS ===================

void World::check_add_tools(vec2 screen) {

//=================check for ice collision
    int collided = 0;
    std::vector<Ice>::iterator itf;
    for (itf = m_freeze.begin(); itf != m_freeze.end();) {
        if (m_player1.collides_with(*itf))
            collided = 1;
        if (m_player2.collides_with(*itf))
            collided = 2;

        if (collided != 0) {
            float index = (float) m_toolboxManager.addItem(1, collided);
            if ((int) index != 100) {
                itf = m_freeze.erase(itf);//m_freeze.begin()+freezecount);
                collect_freeze(*itf, collided, index);
                if (collided == 1) {
                    m_player1.set_mass(m_player1.get_mass() + itf->get_mass());
                }
                if (collided == 2) {
                    m_player2.set_mass(m_player2.get_mass() + itf->get_mass());
                }

            } else
                ++itf;

        } else
            ++itf;
        collided = 0;
    }

//=================check for water collision
    std::vector<Water>::iterator itw;
    for (itw = m_water.begin(); itw != m_water.end();) {
        if (m_player1.collides_with(*itw))//water))
            collided = 1;
        if (m_player2.collides_with(*itw))//water))
            collided = 2;

        if (collided != 0) {
            float index = (float) m_toolboxManager.addItem(2, collided);
            if ((int) index != 100) {
                itw = m_water.erase(itw);
                collect_water(*itw, collided, index);
                if (collided == 1) {
                    m_player1.set_mass(m_player1.get_mass() + itw->get_mass());
                }
                if (collided == 2) {
                    m_player2.set_mass(m_player2.get_mass() + itw->get_mass());
                }
            } else
                ++itw;
        } else
            ++itw;
        collided = 0;
    }


//=================check for mud collision
    std::vector<Mud>::iterator itmud;
    std::vector<Mud>::iterator itmudcheck;
    int collided1 = false;
    int collided2 = false;

    for (itmudcheck = m_mud_collected.begin(); itmudcheck != m_mud_collected.end();) {
        if ((int) difftime(time(0), itmudcheck->mudTime) >= 15) {
            m_player1.set_speed(m_player1.get_originalspeed());
            m_player2.set_speed(m_player2.get_originalspeed());
            itmudcheck->destroy();
            itmudcheck = m_mud_collected.erase(itmudcheck);
        } else
            ++itmudcheck;
    }
    for (itmud = m_mud_collected.begin(); itmud != m_mud_collected.end(); ++itmud) {
        if (m_player1.collides_with(*itmud))
            collided1 = true;
        if (m_player2.collides_with(*itmud))
            collided2 = true;

        if (collided1) {
            bool beingaffected = false;
            for (auto &mud_collected : m_mud_collected) {
                if (mud_collected.is_affected(1))
                    beingaffected = true;
            }
            if (!beingaffected) {
                m_player1.set_originalspeed(m_player1.get_speed());
                //fprintf(stderr, "player1 speed %f \n", m_player1.get_speed());
                m_player1.set_speed(m_player1.get_speed() - (100.f * m_player1.get_mass()));
                if (m_player1.get_speed() <= 0.f)
                    m_player1.set_speed(10.f);
                //fprintf(stderr, "player1 speed affected %f \n", m_player1.get_speed());
                itmud->set_affected(1, true);
            }
        }
        if (collided2) {
            bool beingaffected = false;
            for (auto &mud_collected : m_mud_collected) {
                if (mud_collected.is_affected(2))
                    beingaffected = true;
            }
            if (!beingaffected) {
                m_player2.set_originalspeed(m_player2.get_speed());
                //fprintf(stderr, "player2 speed %f \n", m_player2.get_speed());
                m_player2.set_speed(m_player2.get_speed() - (100.f * m_player2.get_mass()));
                if (m_player2.get_speed() <= 0.f)
                    m_player2.set_speed(10.f);
                //fprintf(stderr, "player2 speed affected %f \n", m_player2.get_speed());
                itmud->set_affected(2, true);
            }
        }

        if (!collided1) {
            itmud->set_affected(1, false);
            bool beingaffected = false;
            for (auto &mud_collected : m_mud_collected) {
                if (mud_collected.is_affected(1))
                    beingaffected = true;
            }
            if (!beingaffected)
                m_player1.set_speed(m_player1.get_originalspeed());
        }
        if (!collided2) {
            itmud->set_affected(2, false);
            bool beingaffected = false;
            for (auto &mud_collected : m_mud_collected) {
                if (mud_collected.is_affected(2))
                    beingaffected = true;
            }
            if (!beingaffected)
                m_player2.set_speed(m_player2.get_originalspeed());
        }

        if (armourInUse_p1)
            m_player1.set_speed(m_player1.get_originalspeed());
        if (armourInUse_p2)
            m_player2.set_speed(m_player2.get_originalspeed());

        collided1 = false;
        collided2 = false;
    }

//=================check for bomb collision
    std::vector<Bomb>::iterator itb;
    for (itb = m_bomb.begin(); itb != m_bomb.end();) {
        if (m_player1.collides_with(*itb))
            collided = 1;
        if (m_player2.collides_with(*itb))
            collided = 2;

        if (collided != 0) {
            float index = (float) m_toolboxManager.addItem(5, collided);
            if ((int) index != 100) {
                itb = m_bomb.erase(itb);
                collect_bomb(*itb, collided, index);
                if (collided == 1) {
                    m_player1.set_mass(m_player1.get_mass() + itb->get_mass());
                    m_player1.create_blood(m_player1.get_position());
                }
                if (collided == 2) {
                    m_player2.set_mass(m_player2.get_mass() + itb->get_mass());
                }
            } else
                ++itb;
        } else
            ++itb;
        collided = 0;
    }

//=================check for missile collision
    std::vector<Missile>::iterator itm;
    for (itm = m_missile.begin(); itm != m_missile.end();) {
        if (m_player1.collides_with(*itm))
            collided = 1;
        if (m_player2.collides_with(*itm))
            collided = 2;

        if (collided != 0) {
            float index = (float) m_toolboxManager.addItem(6, collided);
            if ((int) index != 100) {
                itm = m_missile.erase(itm);
                collect_missile(*itm, collided, index);
                if (collided == 1) {
                    m_player1.set_mass(m_player1.get_mass() + itm->get_mass());
                }
                if (collided == 2) {
                    m_player2.set_mass(m_player2.get_mass() + itm->get_mass());
                }
            } else
                ++itm;
        } else
            ++itm;
        collided = 0;
    }

//=================check for armour collision
    std::vector<Armour>::iterator ita;
    for (ita = m_armour.begin(); ita != m_armour.end();) {
        if (m_player1.collides_with(*ita))
            collided = 1;
        if (m_player2.collides_with(*ita))
            collided = 2;

        if (collided != 0) {
            float index = (float) m_toolboxManager.addItem(7, collided);
            if ((int) index != 100) {
                ita = m_armour.erase(ita);
                collect_armour(*ita, collided, index);
                if (collided == 1) {
                    m_player1.set_mass(m_player1.get_mass() + ita->get_mass());
                }
                if (collided == 2) {
                    m_player2.set_mass(m_player2.get_mass() + ita->get_mass());
                }
            } else
                ++ita;
        } else
            ++ita;
        collided = 0;
    }

//=================check for antidote collision
    if (m_player1.collides_with(m_antidote)) {
        // 5 sec delay before players can pick up tools again
        if ((int) difftime(time(0), droppedAntidoteTime_p1) >= 5)
            collided = 1;
    }
    if (m_player2.collides_with(m_antidote)) {
        if ((int) difftime(time(0), droppedAntidoteTime_p2) >= 5)
            collided = 2;
    }

    if (collided != 0 && m_antidote.belongs_to == 0) {
        //fprintf(stderr, "collided \n");
        float index = (float) m_toolboxManager.addItem(3, collided);
        if ((int) index != 100) {
            if (collided == 1)
                m_antidote.belongs_to = 1;
            if (collided == 2)
                m_antidote.belongs_to = 2;
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, collided));
            m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
        }
    }

//=================check for limbs collision
    string checklegs;
    checklegs = m_limbsManager.check_collision_with_players(&m_player1, &m_player2, &m_toolboxManager);

    if (checklegs == "1leg"){
        if (m_limbsManager.getCollectedLegs(1) == 1)
            leg_times_1 = time(0);
        //fprintf(stderr, "1leg");
    }
    if (checklegs == "2leg"){
        if (m_limbsManager.getCollectedLegs(2) == 1)
            leg_times_2 = time(0);
        //fprintf(stderr, "2leg");
    }
//    if (collided != 0) {
//        if (collided <= 2) {
//            m_toolboxManager.addSlot(collided);
//        }
//        else {
//            m_toolboxManager.addSlot(1);
//            m_toolboxManager.addSlot(2);
//        }
//    }

    //TODO check collision with zombies
}

// =========== COLLECT AND SET TOOLS ===================
void World::collect_freeze(Ice freeze, int player, float index) {
    if (player == 1) {
        m_freeze_collected_1.emplace_back(freeze);
        Ice &new_freeze = m_freeze_collected_1.back();
        new_freeze.set_position(m_toolboxManager.new_tool_position(index, player));
        new_freeze.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
    if (player == 2) {
        m_freeze_collected_2.emplace_back(freeze);
        Ice &new_freeze = m_freeze_collected_2.back();
        new_freeze.set_position(m_toolboxManager.new_tool_position(index, player));
        new_freeze.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }

}

void World::collect_water(Water water, int player, float index) {
    if (player == 1) {
        m_water_collected_1.emplace_back(water);
        Water &new_water = m_water_collected_1.back();
        new_water.set_position(m_toolboxManager.new_tool_position(index, player));
        new_water.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
    if (player == 2) {
        m_water_collected_2.emplace_back(water);
        Water &new_water = m_water_collected_2.back();
        new_water.set_position(m_toolboxManager.new_tool_position(index, player));
        new_water.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
}

void World::collect_bomb(Bomb bomb, int player, float index) {
    if (player == 1) {
        m_bomb_collected_1.emplace_back(bomb);
        Bomb &new_bomb = m_bomb_collected_1.back();
        new_bomb.set_position(m_toolboxManager.new_tool_position(index, player));
    }
    if (player == 2) {
        m_bomb_collected_2.emplace_back(bomb);
        Bomb &new_bomb = m_bomb_collected_2.back();
        new_bomb.set_position(m_toolboxManager.new_tool_position(index, player));
    }
}

void World::collect_armour(Armour armour, int player, float index) {
    if (player == 1) {
        m_armour_collected_1.emplace_back(armour);
        Armour &new_armour = m_armour_collected_1.back();
        new_armour.set_position(m_toolboxManager.new_tool_position(index, player));
        new_armour.set_scale({-8.5f * ViewHelper::getRatio(), 8.5f * ViewHelper::getRatio()});
    }
    if (player == 2) {
        m_armour_collected_2.emplace_back(armour);
        Armour &new_armour = m_armour_collected_2.back();
        new_armour.set_position(m_toolboxManager.new_tool_position(index, player));
        new_armour.set_scale({-8.f * ViewHelper::getRatio(), 8.f * ViewHelper::getRatio()});
    }
}

void World::collect_missile(Missile missile, int player, float index) {
    if (player == 1) {
        m_missile_collected_1.emplace_back(missile);
        Missile &new_missile = m_missile_collected_1.back();
        new_missile.set_position(m_toolboxManager.new_tool_position(index, player));
        new_missile.set_scale({-3.5f * ViewHelper::getRatio(), 3.5f * ViewHelper::getRatio()});
    }
    if (player == 2) {
        m_missile_collected_2.emplace_back(missile);
        Missile &new_missile = m_missile_collected_2.back();
        new_missile.set_position(m_toolboxManager.new_tool_position(index, player));
        new_missile.set_scale({-3.5f * ViewHelper::getRatio(), 3.5f * ViewHelper::getRatio()});
    }
}


void World::use_tool_1(int tool_number) {
    int dropAntidote = false;
    if (droptool_p1 && m_antidote.belongs_to == 1) {
        tool_number = 3;
        dropAntidote = true;
    }

    if (tool_number == 1) {
        if (!armourInUse_p2 && !droptool_p1) {
            m_player2.set_freezestate(true);
            immobilize = m_freeze_collected_1.front().use_freeze(2);
            freezeTime = time(0);
        }
        m_player1.set_mass(m_player1.get_mass() - m_freeze_collected_1.begin()->get_mass());
        m_freeze_collected_1.erase(m_freeze_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);

    }
    if (tool_number == 2) {
        if (!droptool_p1) {
            Mud mud;
            if (mud.init()) {
                mud.set_position(m_player1.get_position());
                m_mud_collected.emplace_back(mud);
                m_mud_collected.back().mudTime = time(0);
            }
        }
        m_player1.set_mass(m_player1.get_mass() - m_water_collected_1.begin()->get_mass());
        m_water_collected_1.erase(m_water_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }

    //antidote is the first tool, move it to the back
    if (tool_number == 3) {
        if (!droptool_p1) {
            m_toolboxManager.decreaseSlot(1);
            m_toolboxManager.addSlot(1);

            float index = (float) m_toolboxManager.addItem(3, 1);
            if ((int) index != 100) {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
                m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
            }
        } else {
            std::vector<int> list = m_toolboxManager.getListOfSlot_1();
            for (int i = 0; i < list.size(); i++) {
                if (list.at(i) == 3)
                    m_toolboxManager.antidotePos = i;
            }
            m_toolboxManager.decreaseSlot(1);
            m_antidote.set_position(m_player1.get_position());
            m_antidote.set_scale({-0.10f * ViewHelper::getRatio(), 0.10f * ViewHelper::getRatio()});
            m_antidote.belongs_to = 0;
            droppedAntidoteTime_p1 = time(0);
            //fprintf(stderr, "dropped antidote 1: %d \n", m_antidote.belongs_to);
        }

    }
/*    if (tool_number == 4) {
//        m_player1.set_mass(m_player1.get_mass() - m_legs_collected_1.begin()->get_mass());
//        m_legs_collected_1.erase(m_legs_collected_1.begin());
        m_limbsManager.decreaseCollectedLegs(1);
        m_toolboxManager.decreaseSlot(1);
        m_player1.increase_speed_legs(-10);
    }*/
    if (tool_number == 5) {
        if (!droptool_p1) {
            used_bombs.emplace_back(m_bomb_collected_1.front());
            Bomb &use_bomb = used_bombs.back();
            use_bomb.set_position(m_player1.get_position());
            useBomb = true;
            use_bomb.bombTime = time(0);

            float xspeed = 0.f;
            float yspeed = 0.f;
            vec2 shootdirection = m_player1.get_shootDirection();
            if (shootdirection.y == 0) //up
                yspeed = -100.f;
            if (shootdirection.x == 1) //left
                xspeed = -200.f;
            if (shootdirection.y == 2) //down
                yspeed = 100.f;
            if (shootdirection.x == 3) //right
                xspeed = 200.f;

            use_bomb.set_speed({xspeed, yspeed});
        }

        //use_bomb.set_speed({200, 100});
        m_player1.set_mass(m_player1.get_mass() - m_bomb_collected_1.front().get_mass());
        m_bomb_collected_1.erase(m_bomb_collected_1.begin());

        m_toolboxManager.decreaseSlot(1);
    }
    if (tool_number == 6) {
        if (!droptool_p1)
        {
            used_missiles.emplace_back(m_missile_collected_1.front());
            Missile &use_missile = used_missiles.back();
            use_missile.set_position(m_player1.get_position());
            useMissile = true;
            use_missile.useMissileOnPlayer = 2;
            use_missile.onPlayerPos = m_player2.get_position();
            
            vec2 missileDir = direction(use_missile.get_position(),m_player2.get_position());
            
            float angle = atan2(missileDir.x, missileDir.y);
            
            use_missile.set_rotation(angle);
            //float x = (m_player2.get_position().x- use_missile.get_position().x);
            //std::fabs(( m_player2.get_position().x - use_missile.get_position().x));
            //float y = (m_player2.get_position().y - use_missile.get_position().y);
            //std::fabs((m_player2.get_position().y - use_missile.get_position().y));
            
            use_missile.set_speed(missileDir);
        }
        m_player1.set_mass(m_player1.get_mass() - m_missile_collected_1.begin()->get_mass());
        m_missile_collected_1.erase(m_missile_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }
    if (tool_number == 7) {
        if (!droptool_p1) {
            armourInUse_p1 = true;
            m_player1.set_armourstate(true);
            armourTime_p1 = time(0);
        }
        m_player1.set_mass(m_player1.get_mass() - m_armour_collected_1.begin()->get_mass());
        m_armour_collected_1.erase(m_armour_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }
    droptool_p1 = false;
    shift_1(dropAntidote);
}

void World::shift_1(bool droppedAntidote) {
    std::vector<int> list = m_toolboxManager.getListOfSlot_1();
    std::vector<int>::iterator it;
    int freezecount = 0;
    int watercount = 0;
    //int legcount = 0;
    int bombcount = 0;
    int armourcount = 0;
    int missilecount = 0;
    int antidotePos = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it) {

        if (m_toolboxManager.antidotePos == antidotePos && droppedAntidote) {
            droppedAntidote = false;
            m_toolboxManager.antidotePos = 0;
        }
        if (*it == 1) {
            if (!droppedAntidote) {
                Ice &freeze = m_freeze_collected_1.at(freezecount);
                freeze.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            freezecount++;
        }
        if (*it == 2) {
            if (!droppedAntidote) {
                Water &water = m_water_collected_1.at(watercount);
                water.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            watercount++;
        }
        if (*it == 3) {
            if (!droppedAntidote) {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
        }
        /*if (*it == 4) {
            if (!droppedAntidote) {
                m_limbsManager.shiftCollectedLegs(1, &m_toolboxManager, index, legcount);
            }
            legcount++;
        }*/
        if (*it == 5) {
            if (!droppedAntidote) {
                Bomb &bomb = m_bomb_collected_1.at(bombcount);
                bomb.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            bombcount++;
        }
        if (*it == 6) {
            if (!droppedAntidote) {
                Missile &missile = m_missile_collected_1.at(missilecount);
                missile.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            missilecount++;
        }
        if (*it == 7) {
            if (!droppedAntidote) {
                Armour &armour = m_armour_collected_1.at(armourcount);
                armour.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            armourcount++;
        }
        index++;
        antidotePos++;
    }
}

void World::use_tool_2(int tool_number) {
    int dropAntidote = false;
    if (droptool_p2 && m_antidote.belongs_to == 2) {
        tool_number = 3;
        dropAntidote = true;
    }

    if (tool_number == 1) {
        if (!armourInUse_p1 && !droptool_p2) {
            m_player1.set_freezestate(true);
            immobilize = m_freeze_collected_1.front().use_freeze(1);
            freezeTime = time(0);
        }
        m_player2.set_mass(m_player2.get_mass() - m_freeze_collected_2.begin()->get_mass());
        m_freeze_collected_2.erase(m_freeze_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 2) {
        if (!droptool_p2) {
            Mud mud;
            if (mud.init()) {
                mud.set_position(m_player2.get_position());
                m_mud_collected.emplace_back(mud);
                m_mud_collected.back().mudTime = time(0);
            }
        }
        m_player2.set_mass(m_player2.get_mass() - m_water_collected_2.begin()->get_mass());
        m_water_collected_2.erase(m_water_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }

    if (tool_number == 3) {
        if (!droptool_p2) {
            m_toolboxManager.decreaseSlot(2);
            m_toolboxManager.addSlot(2);

            float index = (float) m_toolboxManager.addItem(3, 2);
            if ((int) index != 100) {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
                m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
            }
        } else {
            std::vector<int> list = m_toolboxManager.getListOfSlot_2();
            for (int i = 0; i < list.size(); i++) {
                if (list.at(i) == 3)
                    m_toolboxManager.antidotePos = i;
            }
            m_toolboxManager.decreaseSlot(2);
            m_antidote.set_position(m_player2.get_position());
            m_antidote.set_scale({-0.10f * ViewHelper::getRatio(), 0.10f * ViewHelper::getRatio()});
            m_antidote.belongs_to = 0;
            droppedAntidoteTime_p2 = time(0);
            //fprintf(stderr, "dropped antidote 2: %d \n", m_antidote.belongs_to);
        }
    }
    /*if (tool_number == 4) {
        m_limbsManager.decreaseCollectedLegs(2);
        m_toolboxManager.decreaseSlot(2);
        m_player2.increase_speed_legs(-10);
    }*/
    if (tool_number == 5) {
        if (!droptool_p2) {
            used_bombs.emplace_back(m_bomb_collected_2.front());
            Bomb &use_bomb = used_bombs.back();

            use_bomb.set_position(m_player2.get_position());
            useBomb = true;
            use_bomb.bombTime = time(0);

            float xspeed = 0.f;
            float yspeed = 0.f;
            vec2 shootdirection = m_player2.get_shootDirection();
            if (shootdirection.y == 0) //up
                yspeed = -100.f;
            if (shootdirection.x == 1) //left
                xspeed = -200.f;
            if (shootdirection.y == 2) //down
                yspeed = 100.f;
            if (shootdirection.x == 3) //right
                xspeed = 200.f;

            use_bomb.set_speed({xspeed, yspeed});

        }
        m_player2.set_mass(m_player2.get_mass() - m_bomb_collected_2.front().get_mass());
        m_bomb_collected_2.erase(m_bomb_collected_2.begin());

        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 6) {
        if (!droptool_p2)
        {
            used_missiles.emplace_back(m_missile_collected_2.front());
            Missile &use_missile = used_missiles.back();
            use_missile.set_position(m_player2.get_position());
            useMissile = true;
            use_missile.useMissileOnPlayer = 1;
            use_missile.onPlayerPos = m_player1.get_position();
            
            vec2 missileDir = direction(use_missile.get_position(),m_player1.get_position());
            float angle = atan2(missileDir.x, missileDir.y);
            
            use_missile.set_rotation(angle);
            use_missile.set_speed(missileDir);
        }
        m_player2.set_mass(m_player2.get_mass() - m_missile_collected_2.begin()->get_mass());
        m_missile_collected_2.erase(m_missile_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 7) {
        if (!droptool_p2) {
            armourInUse_p2 = true;
            m_player2.set_armourstate(true);
            armourTime_p2 = time(0);
        }
        m_player2.set_mass(m_player2.get_mass() - m_armour_collected_2.begin()->get_mass());
        m_armour_collected_2.erase(m_armour_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    droptool_p2 = false;
    shift_2(dropAntidote);

}

void World::shift_2(bool droppedAntidote) {
    std::vector<int> list = m_toolboxManager.getListOfSlot_2();
    std::vector<int>::iterator it;
    int freezecount = 0;
    int watercount = 0;
    //int legcount = 0;
    int bombcount = 0;
    int armourcount = 0;
    int missilecount = 0;
    int antidotePos = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it) {
        if (m_toolboxManager.antidotePos == antidotePos && droppedAntidote) {
            droppedAntidote = false;
            m_toolboxManager.antidotePos = 0;
        }

        if (*it == 1) {
            if (!droppedAntidote) {
                Ice &freeze = m_freeze_collected_2.at(freezecount);
                freeze.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            freezecount++;
        }
        if (*it == 2) {
            if (!droppedAntidote) {
                Water &water = m_water_collected_2.at(watercount);
                water.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            watercount++;
        }
        if (*it == 3) {
            if (!droppedAntidote) {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
        }
        /*if (*it == 4) {
            if (!droppedAntidote) {
                m_limbsManager.shiftCollectedLegs(2, &m_toolboxManager, index, legcount);
            }
            legcount++;
        }*/
        if (*it == 5) {
            if (!droppedAntidote) {
                Bomb &bomb = m_bomb_collected_2.at(bombcount);
                bomb.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            bombcount++;
        }
        if (*it == 6) {
            if (!droppedAntidote) {
                Missile &missile = m_missile_collected_2.at(missilecount);
                missile.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            missilecount++;
        }
        if (*it == 7) {
            if (!droppedAntidote) {
                Armour &armour = m_armour_collected_2.at(armourcount);
                armour.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            armourcount++;
        }
        index++;
        antidotePos++;
    }
}

// =========== EXPLODING LOGIC ===================

void World::use_bomb(float ms) {
    std::vector<Bomb>::iterator itbomb;
    std::vector<Bomb>::iterator checkbomb;
    int count = 0;

    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    for (itbomb = used_bombs.begin(); itbomb != used_bombs.end();) {
        itbomb->set_speed({itbomb->get_speed().x * (0.997f), itbomb->get_speed().y * (0.997f)});


        if ((std::fabs(itbomb->get_speed().x) <= 50 && std::fabs(itbomb->get_speed().y) <= 50)) {
            itbomb->set_speed({0.f, 0.f});
            autoExplode(*itbomb, count);
        }
        else if (m_player1.collides_with(*itbomb)||m_player2.collides_with(*itbomb)){
            if ((int) difftime(time(0), itbomb->bombTime) >= 2) {
                itbomb->set_speed({0.f, 0.f});
                autoExplode(*itbomb, count);
            }
            else {
                itbomb->move({itbomb->get_speed().x * (ms / 1000),
                    itbomb->get_speed().y * (ms / 1000)}, false);
                itbomb->checkBoundaryCollision(width, height, ms, mapCollisionPoints);

                for (checkbomb = used_bombs.begin(); checkbomb != used_bombs.end() - 1; ++checkbomb) {
                    if (checkbomb != itbomb) {
                        if (itbomb->collides_with(*checkbomb))
                            itbomb->checkCollision(*checkbomb, ms);
                    }
                }

                ++itbomb;
                ++count;
            }
        }
        else {
            itbomb->move({itbomb->get_speed().x * (ms / 1000),
                itbomb->get_speed().y * (ms / 1000)}, false);
            itbomb->checkBoundaryCollision(width, height, ms, mapCollisionPoints);

            for (checkbomb = used_bombs.begin(); checkbomb != used_bombs.end() - 1; ++checkbomb) {
                if (checkbomb != itbomb) {
                    if (itbomb->collides_with(*checkbomb))
                        itbomb->checkCollision(*checkbomb, ms);
                }
            }

            ++itbomb;
            ++count;
        }
    }

}

void World::autoExplode(Bomb bomb, int position) {
    float force_p1 = 0;
    float force_p2 = 0;
    //if (!armourInUse_p1) {
        force_p1 = bomb.get_force(m_player1.get_mass(),
                                                m_player1.get_speed(),
                                                m_player1.get_position());
    //}

    //if (!armourInUse_p2) {
        force_p2 = bomb.get_force(m_player2.get_mass(),
                                                m_player2.get_speed(),
                                                m_player2.get_position());
    //}
    if (armourInUse_p1){
        force_p1 = 0;
        armourInUse_p1 = false;
        m_player1.set_armourstate(false);
    }
    if (armourInUse_p2) {
        force_p2 = 0;
        armourInUse_p2 = false;
        m_player2.set_armourstate(false);
    }

    if (force_p1 > 0) {
        m_player1.set_blowback(true);
        m_player1.set_speed(force_p1);
        m_player1.set_blowbackForce({(m_player1.get_position().x - bomb.get_position().x),
                                     (m_player1.get_position().y - bomb.get_position().y)});
        explosion = true;
        droptool_p1 = true;
        use_tool_1(m_toolboxManager.useItem(1));
    }
    if (force_p2 > 0) {
        m_player2.set_blowback(true);
        m_player2.set_speed(force_p2);
        m_player2.set_blowbackForce({(m_player2.get_position().x - bomb.get_position().x),
                                     (m_player2.get_position().y - bomb.get_position().y)});
        explosion = true;
        droptool_p2 = true;
        use_tool_2(m_toolboxManager.useItem(2));
    }

    // create_explosion(used_bombs.begin()->get_position());
    // used_bombs.begin()->explode();
    //fprintf(stderr,"# of bombs: %lu \n", used_bombs.size());
    //fprintf(stderr,"bomb to remove: %d \n", position);
    std::vector<Bomb>::iterator itbomb;
    int i = 0;
    for (itbomb = used_bombs.begin(); itbomb != used_bombs.end();) {
        if (i == position) {
            itbomb = used_bombs.erase(itbomb);
        }
        else{
            ++itbomb;
            ++i;
        }
    }
}

void World::explode() {
    if (m_player1.get_blowback())
        m_player1.set_speed(m_player1.get_speed() * 0.9);
    if (m_player2.get_blowback())
        m_player2.set_speed(m_player2.get_speed() * 0.9);

    if (m_player1.get_speed() <= 5 || m_player2.get_speed() <= 5) {
        m_player1.set_blowback(false);
        explosion = false;
        m_player1.set_speed(m_player1.get_speed_legs());
        m_player2.set_blowback(false);
        explosion = false;
        m_player2.set_speed(m_player2.get_speed_legs());
    }
}

void World::use_missile(float ms) {
    std::vector<Missile>::iterator itmissile;
    int count = 0;
    
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    
    for (itmissile = used_missiles.begin(); itmissile != used_missiles.end();) {
        /*if ((itmissile->useMissileOnPlayer == 1 && m_player1.collides_with(*itmissile))||
            (itmissile->useMissileOnPlayer == 2 && m_player2.collides_with(*itmissile)))*/
        //fprintf(stderr,"# of missiles: %lu", used_missiles.size());
        if (itmissile->checkPoint())
            autoExplodeMissile(*itmissile, count);
        else {
            itmissile->move({itmissile->get_speed().x * (ms / 1000),
                itmissile->get_speed().y * (ms / 1000)});
            /*if (itmissile->checkboundary())
                used_missiles.erase(itmissile);
            else*/
            ++itmissile;
            ++count;
        }
    }
    
}


void World::autoExplodeMissile(Missile missile, int position) {
    float force_p1 = 0;
    float force_p2 = 0;
    //if (!armourInUse_p1) {
        force_p1 = missile.get_force(m_player1.get_mass(),
                                                m_player1.get_speed(),
                                                m_player1.get_position());
    //}
    
    //if (!armourInUse_p2) {
        force_p2 = missile.get_force(m_player2.get_mass(),
                                                m_player2.get_speed(),
                                                m_player2.get_position());
    //}

    if (armourInUse_p1){
        force_p1 = 0;
        armourInUse_p1 = false;
        m_player1.set_armourstate(false);
    }
    if (armourInUse_p2) {
        force_p2 = 0;
        armourInUse_p2 = false;
        m_player2.set_armourstate(false);
    }

    if (force_p1 > 0) {
        m_player1.set_blowback(true);
        m_player1.set_speed(force_p1);
        m_player1.set_blowbackForce({(m_player1.get_position().x - missile.get_position().x),
            (m_player1.get_position().y - missile.get_position().y)});
        explosion = true;
        droptool_p1 = true;
        use_tool_1(m_toolboxManager.useItem(1));
    }
    if (force_p2 > 0) {
        m_player2.set_blowback(true);
        m_player2.set_speed(force_p2);
        m_player2.set_blowbackForce({(m_player2.get_position().x - missile.get_position().x),
            (m_player2.get_position().y - missile.get_position().y)});
        explosion = true;
        droptool_p2 = true;
        use_tool_2(m_toolboxManager.useItem(2));
    }

    //fprintf(stderr,"# of missiles: %lu \n", used_missiles.size());
    //fprintf(stderr,"missile to remove: %d \n", position);
    std::vector<Missile>::iterator itmissile;
    int i = 0;
     for (itmissile = used_missiles.begin(); itmissile != used_missiles.end();) {
        if (i == position) {
            itmissile = used_missiles.erase(itmissile);
        }
        else{
            ++itmissile;
            ++i;
        }
    }
}


void World::populateMapCollisionPoints() {

    mapCollisionPoints.push_back({ 47.3633f * ViewHelper::getRatio(), 371.539f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 40.1484f * ViewHelper::getRatio(), 292.309f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 96.5586f * ViewHelper::getRatio(), 252.488f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 142.387f * ViewHelper::getRatio(), 203.789f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 139.512f * ViewHelper::getRatio(), 181.172f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 176.07f * ViewHelper::getRatio(), 152.285f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 256.551f * ViewHelper::getRatio(), 152.48f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 261.172f * ViewHelper::getRatio(), 137.648f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 334.645f * ViewHelper::getRatio(), 102.211f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 422.746f * ViewHelper::getRatio(), 60.7383f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 601.715f * ViewHelper::getRatio(), 96.6602f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 690.293f * ViewHelper::getRatio(), 96.0742f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 774.266f * ViewHelper::getRatio(), 81.8281f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 794.629f * ViewHelper::getRatio(), 87.1016f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 814.023f * ViewHelper::getRatio(), 63.6406f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 878.555f * ViewHelper::getRatio(), 83.2891f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 958.617f * ViewHelper::getRatio(), 95.7852f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1007.97f * ViewHelper::getRatio(), 122.348f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1062.52f * ViewHelper::getRatio(), 101.793f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1182.62f * ViewHelper::getRatio(), 117.871f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1136.04f * ViewHelper::getRatio(), 138.668f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1134.95f * ViewHelper::getRatio(), 218.684f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1189.22f * ViewHelper::getRatio(), 247.887f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1179.44f * ViewHelper::getRatio(), 290.305f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1223.39f * ViewHelper::getRatio(), 286.859f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1252.36f * ViewHelper::getRatio(), 309.699f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1245.82f * ViewHelper::getRatio(), 379.09f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1172.02f * ViewHelper::getRatio(), 431.262f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1023.17f * ViewHelper::getRatio(), 477.641f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 1043.39f * ViewHelper::getRatio(), 510.52f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 834.137f * ViewHelper::getRatio(), 565.66f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 722.289f * ViewHelper::getRatio(), 554.605f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 598.59f * ViewHelper::getRatio(), 612.293f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 307.602f * ViewHelper::getRatio(), 521.102f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 288.453f * ViewHelper::getRatio(), 457.098f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 256.238f * ViewHelper::getRatio(), 469.301f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 86.1797f * ViewHelper::getRatio(), 393.562f * ViewHelper::getRatio()});
    mapCollisionPoints.push_back({ 91.6211f * ViewHelper::getRatio(), 374.586f * ViewHelper::getRatio()});
}

int World::saveToFile(string winnername) {
    FILE *file;
    int file_exists;
    const char * filename="score.txt";
    
    /*first check if the file exists...*/
    file=fopen(filename,"r");
    if (file==NULL) file_exists=0;
    else {file_exists=1; fclose(file);}
    
    /*...then open it in the appropriate way*/
    if (file_exists==1)
    {
        printf("file exists!\n");
        file=fopen(filename,"r+b");
    }
    else
    {
        printf("file does not exist!\n");
        file=fopen(filename,"w+b");
    }
    
    if (file!=NULL)
    {
        string tempWinnerName = winnername;
        printf ("file opened succesfully!\n");
        string temp = "";
//        temp.append("player2");
//        temp.append(":");
//        temp.append("5\n");
//        temp.append("player1");
//        temp.append(":");
//        temp.append("5\n");
//

        std::vector<std::string> scores = parseFile(file);

        fclose(file);
        file=fopen(filename,"w+b");

        bool winner_exist = false;
//        foreach item, if match the name, parse, and add. else just append.
        for(auto &score : scores) {
            size_t position = score.find(":");
            if(position != std::string::npos) {
                if(tempWinnerName == score.substr(0, position)) {
                    winner_exist = true;
                    string thisScore = score.substr(position+1);
                    int thisScoreInt = std::stoi(thisScore);
                    thisScoreInt++;
                    string thisScoreString = std::to_string(thisScoreInt);
                    temp.append(tempWinnerName);
                    temp.append(":");
                    temp.append(thisScoreString);
                    temp.append("\n");
                } else {
                    temp.append(score);
                    temp.append("\n");
                }
            }
        }

        if (!winner_exist){
            temp.append(tempWinnerName);
            temp.append(":");
            temp.append("1");
            temp.append("\n");
        }

        char toWrite[temp.length() * (sizeof(char)) + 1];
        strncpy(toWrite, temp.c_str(), sizeof(toWrite));
        toWrite[sizeof(toWrite) - 1] = 0;

        fwrite (toWrite , sizeof(char), sizeof(toWrite), file);
        
        fclose(file);
    }

    return 0;
}

std::vector<std::string> World::parseFile(FILE *file) {

    long lSize;
    char * buffer;
    string bufferString;
    size_t result;
    std::vector<std::string> vectorResult;

    fseek (file , 0 , SEEK_END);
    lSize = ftell (file);
    rewind (file);


    std::cout << lSize << std::endl;

    // allocate memory to contain the whole file:
    buffer = (char*) malloc ((sizeof(char))*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,sizeof buffer[0],lSize,file);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}


    bufferString = std::string(buffer);

    string token;
    string delimiter = "\n";
    size_t pos = 0;

    while ((pos = bufferString.find(delimiter)) != std::string::npos) {
        token = bufferString.substr(0, pos);
        std::cout << token <<std::endl;
        bufferString.erase(0, pos + delimiter.length());
        vectorResult.push_back(token);
    }

    for(string vr : vectorResult) {
        std::cout << "vectorResult" << vr << std::endl;
    }



    free(buffer);

    return vectorResult;
}

std::map<std::string, int> World::getHighScores(int numOfHighScores) {
    FILE *file;
    int file_exists;
    const char * filename="score.txt";
    std::map<std::string, int> hsMap;

    /*first check if the file exists...*/
    file=fopen(filename,"r");
    if (file==NULL) file_exists=0;
    else {file_exists=1; fclose(file);}

    /*...then open it in the appropriate way*/
    if (file_exists==1)
    {
        printf("file exists!\n");
        file=fopen(filename,"r+b");
    }
    else
    {
        return hsMap;
    }

    if (file!=NULL)
    {
        std::vector<std::string> scores = parseFile(file);

        for(auto &score : scores) {
            size_t position = score.find(":");
            if(position != std::string::npos) {
                string thisName = score.substr(0, position);
                string thisScore = score.substr(position+1);
                int thisScoreInt = std::stoi(thisScore);

                if(hsMap.size() <= numOfHighScores) {
                    hsMap[thisName] = thisScoreInt;
                } else {
                    map<std::string, int>::iterator it;

                    for ( it = hsMap.begin(); it != hsMap.end(); it++ )
                    {
                        if(thisScoreInt > it->second) {
                            hsMap.erase(it);
                            hsMap[thisName] = thisScoreInt;
                            break;
                        }
                    }
                }

            }
        }

        fclose(file);

        return hsMap;
    }

}



