#include "world.hpp"
#include <sstream>

namespace {
    const size_t MAX_ARMS = 3;
    const size_t MAX_LEGS = 3;
    const size_t MAX_FREEZE = 1;
    const size_t MAX_MISSILE = 1;
    const size_t MAX_ARMOUR = 1;
    const size_t MAX_BOMB = 1;
    const size_t MAX_WATER = 1;
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

    // in this order
    ViewHelper::getInstance(m_window);
    populateMapCollisionPoints();
    mapGrid = new MapGrid((unsigned) screen.x / 100, (unsigned) screen.y / 100);
    m_limbsManager.init(screen, mapCollisionPoints);

    game_started = false;
    game_over = false;
    return m_button.init();
}

void World::destroy() {
    m_worldtexture.destroy();
    m_toolboxManager.destroy();
    m_player1.destroy();
    m_player2.destroy();
    m_zombie.destroy();
    m_tree.destroy();
    m_limbsManager.destroy();
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

    
    for (auto &bomb_used : used_bombs)
        bomb_used.destroy();
    for (auto &mud_collected : m_mud_collected)
        mud_collected.destroy();
    
    m_freeze.clear();
    m_water.clear();
    m_missile.clear();
    m_armour.clear();
    m_bomb.clear();
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
    m_mud_collected.clear();
}

bool World::update(float elapsed_ms) {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    vec2 screen = {(float) w, (float) h};
    if (!game_started) {
        if (m_button.is_clicked()) {
            game_started = true;
            m_button.destroy();

            explosion = false;
            m_min = 2;
            m_sec = 0;
            timeDelay = 5;
            start = time(0);
            immobilize = 0;
            armourInUse_p1 = false;
            armourInUse_p2 = false;
            droptool_p1 = false;
            droptool_p2 = false;
            m_worldtexture.init(screen);
            m_toolboxManager.init({screen.x, screen.y});
            useBomb = false;
            
            //mud.init();
            
            m_player1.init(screen, mapCollisionPoints) && m_player2.init(screen, mapCollisionPoints) &&
            m_antidote.init(screen);
        }
    }

    if (game_started) {
        m_player1.update(elapsed_ms);
        m_player2.update(elapsed_ms);

        random_spawn(elapsed_ms, {screen.x * ViewHelper::getRatio(), screen.y * ViewHelper::getRatio()});
        m_limbsManager.computePaths(elapsed_ms, *mapGrid);

        if ((int) difftime(time(0), start) == timeDelay)
            timer_update();

        check_add_tools(screen);

        if (explosion)
            explode();

        if (useBomb)
            use_bomb(elapsed_ms);
        
        if ((int) difftime(time(0), armourTime_p1) >= 5) {
            armourInUse_p1 = false;
            armourTime_p1 = 0;
        }
        if ((int) difftime(time(0), armourTime_p2) >= 5) {
            armourInUse_p2 = false;
            armourTime_p2 = 0;
        }

        return true;
    }
    return true;
}

void World::timer_update() {

    if (m_min == 0 && m_sec == 0) {
        game_over = true;
        fprintf(stderr, "winner is %d \n", m_antidote.belongs_to);
        destroy();
        m_button.init();
        game_started = false;
        m_min = 0;
        m_sec = 0;
    } else if (m_sec == 0) {
        m_sec = 59;
        m_min -= 1;
        timeDelay++;
    } else {
        m_sec -= 1;
        timeDelay++;
    }

}

// Render our game world
void World::draw() {
    gl_flush_errors();

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    std::stringstream title_ss;
    if (m_sec < 10)
        title_ss << "Time remaining " << m_min << ":" << "0" << m_sec;
    else
        title_ss << "Time remaining " << m_min << ":" << m_sec;
    glfwSetWindowTitle(m_window, title_ss.str().c_str());

    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);
    float left = 0.f;        // *-0.5;
    float top = 0.f;         // (float)h * -0.5;
    float right = (float) w;  // *0.5;
    float bottom = (float) h; // *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);

    mat3 projection_2D{{sx, 0.f, 0.f},
                       {0.f, sy, 0.f},
                       {tx, ty,  1.f}};

    if (!game_started) {
        const float clear_color[3] = {0.3f, 0.3f, 0.8f};
        glClearColor(clear_color[1], clear_color[1], clear_color[1], 1.0);
        glClearDepth(1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_button.draw(projection_2D);
    } else {
        m_worldtexture.draw(projection_2D);
        m_toolboxManager.draw(projection_2D);
        m_antidote.draw(projection_2D);
        m_limbsManager.draw(projection_2D);
        for (auto &freeze : m_freeze)
            freeze.draw(projection_2D);
        for (auto &water : m_water)
            water.draw(projection_2D);
        for (auto &missile : m_missile)
            missile.draw(projection_2D);
        for (auto &armour : m_armour)
            armour.draw(projection_2D);
        for (auto &bomb : m_bomb)
            bomb.draw(projection_2D);

        for (auto &water_collected : m_water_collected_1)
            water_collected.draw(projection_2D);
        for (auto &water_collected : m_water_collected_2)
            water_collected.draw(projection_2D);

        for (auto &freeze_collected : m_freeze_collected_1)
            freeze_collected.draw(projection_2D);
        for (auto &freeze_collected : m_freeze_collected_2)
            freeze_collected.draw(projection_2D);
        for (auto &missile_collected : m_missile_collected_1)
            missile_collected.draw(projection_2D);
        for (auto &missile_collected : m_missile_collected_2)
            missile_collected.draw(projection_2D);
        for (auto &bomb_collected : m_bomb_collected_1)
            bomb_collected.draw(projection_2D);
        for (auto &bomb_collected : m_bomb_collected_2)
            bomb_collected.draw(projection_2D);
        for (auto &armour_collected : m_armour_collected_1)
            armour_collected.draw(projection_2D);
        for (auto &armour_collected : m_armour_collected_2)
            armour_collected.draw(projection_2D);
        for (auto &bomb_used : used_bombs)
            bomb_used.draw(projection_2D);
        for (auto &mud_collected : m_mud_collected)
            mud_collected.draw(projection_2D);

        m_player1.draw(projection_2D);
        m_player2.draw(projection_2D);
        //mud.draw(projection_2D);
    }

    // Presenting
    glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over() const {
    return glfwWindowShouldClose(m_window);
}

// On key callback
void World::on_key(GLFWwindow *, int key, int, int action, int mod) {

    // player1 actions
    if (immobilize != 1 && !m_player1.get_blowback()) {
        if (action == GLFW_PRESS && key == GLFW_KEY_UP)
            m_player1.set_key(0, true);
        if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
            m_player1.set_key(1, true);
        if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
            m_player1.set_key(2, true);
        if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
            m_player1.set_key(3, true);
        if (action == GLFW_RELEASE && key == GLFW_KEY_UP)
            m_player1.set_key(0, false);
        if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT)
            m_player1.set_key(1, false);
        if (action == GLFW_RELEASE && key == GLFW_KEY_DOWN)
            m_player1.set_key(2, false);
        if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT)
            m_player1.set_key(3, false);
        
        if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT_SHIFT)
            use_tool_1(m_toolboxManager.useItem(1));
    }
    if (immobilize == 1 || m_player1.get_blowback()) //player is frozen
    {
        m_player1.set_freezestate(true);
        //fprintf(stderr, "frozen");
        m_player1.set_key(0, false);
        m_player1.set_key(1, false);
        m_player1.set_key(2, false);
        m_player1.set_key(3, false);
        if ((int) difftime(time(0), freezeTime) >= 5) {
            immobilize = 0;
            m_player1.set_freezestate(false);
            freezeTime = 0;
            //fprintf(stderr, "start");
        }

    }

    // player2 actions
    if (immobilize != 2 && !m_player2.get_blowback()) {
        if (action == GLFW_PRESS && key == GLFW_KEY_W)
            m_player2.set_key(0, true);
        if (action == GLFW_PRESS && key == GLFW_KEY_A)
            m_player2.set_key(1, true);
        if (action == GLFW_PRESS && key == GLFW_KEY_S)
            m_player2.set_key(2, true);
        if (action == GLFW_PRESS && key == GLFW_KEY_D)
            m_player2.set_key(3, true);
        if (action == GLFW_RELEASE && key == GLFW_KEY_W)
            m_player2.set_key(0, false);
        if (action == GLFW_RELEASE && key == GLFW_KEY_A)
            m_player2.set_key(1, false);
        if (action == GLFW_RELEASE && key == GLFW_KEY_S)
            m_player2.set_key(2, false);
        if (action == GLFW_RELEASE && key == GLFW_KEY_D)
            m_player2.set_key(3, false);

        // use tools
        if (action == GLFW_PRESS && key == GLFW_KEY_Q)
            use_tool_2(m_toolboxManager.useItem(2));

    }
    if (immobilize == 2 || m_player2.get_blowback()) //player is frozen
    {
        m_player2.set_freezestate(true);
        m_player2.set_key(0, false);
        m_player2.set_key(1, false);
        m_player2.set_key(2, false);
        m_player2.set_key(3, false);
        if ((int) difftime(time(0), freezeTime) >= 5) {
            m_player2.set_freezestate(false);
            immobilize = 0;
            freezeTime = 0;
            //fprintf(stderr, "start");
        }
    }
}

void World::on_mouse_move(GLFWwindow *window, int button, int action, int mod) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if(!game_started) {
        if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
            m_button.clickicon();
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            m_button.click();
            draw();
        }
    } else {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            
            std::cout << "mapCollisionPoints.push_back({ " << xpos << "f * ViewHelper::getRatio(), " << ypos << "f * ViewHelper::getRatio()});" << std::endl;
            // std::cout << "xpos: " << xpos << std::endl;
            // std::cout << "ypos: " << ypos << std::endl;
            // std::cout << "player pos: " << m_player1.get_position().x << ", " << m_player1.get_position().y << std::endl;
            // if (isInsidePolygon(mapCollisionPoints, {(float)xpos * ViewHelper::getRatio(), (float)ypos * ViewHelper::getRatio()})) {
            //     std::cout << "yes it's inside polygon" << std::endl;
            // } else {
            //     std::cout << "nope, it's outside the polygon" << std::endl;
            // }
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

    srand((unsigned) time(0));
    int randNum = rand() % (1000);

    if (randNum % 7 == 0) {
        if (m_limbsManager.get_arms_size() <= MAX_ARMS && m_next_arm_spawn < 0.f) {
            if (!(m_limbsManager.spawn_arms()))
                return false;
            m_next_arm_spawn = (ARM_DELAY_MS / 2) + rand() % (1000);
        }
    }

    if (randNum % 19 == 0) {
        if (m_limbsManager.get_legs_size() <= MAX_LEGS && m_next_leg_spawn < 0.f) {
            if (!(m_limbsManager.spawn_legs()))
                return false;
            m_next_leg_spawn = (LEG_DELAY_MS / 2) + rand() % (1000);
        }
    }
    if (randNum % 13 == 0) {
        if (m_freeze.size() <= MAX_FREEZE && m_next_spawn < 0.f) {
            if (!spawn_freeze())
                return false;
            Ice &new_freeze = m_freeze.back();
            new_freeze.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }
    if (randNum % 21 == 0) {
        if (m_missile.size() <= MAX_MISSILE && m_next_spawn < 0.f) {
            if (!spawn_missile())
                return false;

            Missile &new_missile = m_missile.back();
            new_missile.set_position({(float) ((rand() % (int) screen.x)),
                                      (float) ((rand() % (int) screen.y))});


            new_missile.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }

    if (randNum % 11 == 0) {
        if (m_armour.size() <= MAX_ARMOUR && m_next_spawn < 0.f) {
            if (!spawn_armour())
                return false;

            Armour &new_armour = m_armour.back();
            // new_armour.set_position({(float)((rand() % (int)screen.x)),
            //     (float)((rand() % (int)screen.y))});


            new_armour.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }

    if (randNum % 3 == 0) {
        if (m_bomb.size() <= MAX_BOMB && m_next_spawn < 0.f) {
            if (!spawn_bomb())
                return false;

            Bomb &new_bomb = m_bomb.back();
            // new_bomb.set_position({(float)((rand() % (int)screen.x)),
            //     (float)((rand() % (int)screen.y))});

            new_bomb.set_position(getRandomPointInMap(mapCollisionPoints, screen));

            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }

    if (randNum % 9 == 0) {
        if (m_water.size() <= MAX_WATER && m_next_spawn < 0.f) {
            if (!spawn_water())
                return false;
            Water &new_water = m_water.back();
            new_water.set_position({(float) ((rand() % (int) screen.x)),
                                    (float) ((rand() % (int) screen.y))});

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
        if ((int) difftime(time(0), itmudcheck->mudTime) >= 15)
        {
            m_player1.set_speed(m_player1.get_originalspeed());
            m_player2.set_speed(m_player2.get_originalspeed());
            itmudcheck->destroy();
            itmudcheck = m_mud_collected.erase(itmudcheck);
        }
        else
            ++itmudcheck;
    }
    for (itmud = m_mud_collected.begin(); itmud != m_mud_collected.end(); ++itmud) {
        if (m_player1.collides_with(*itmud))
            collided1 = true;
        if (m_player2.collides_with(*itmud))
            collided2 = true;
        
        if (collided1) {
            bool beingaffected = false;
            for (auto& mud_collected : m_mud_collected)
            {
                if (mud_collected.is_affected(1))
                    beingaffected = true;
            }
            if (!beingaffected)
            {
                m_player1.set_originalspeed(m_player1.get_speed());
                //fprintf(stderr, "player1 speed %f \n", m_player1.get_speed());
                m_player1.set_speed(m_player1.get_speed()-(100.f*m_player1.get_mass()));
                if (m_player1.get_speed() <= 0.f)
                    m_player1.set_speed(10.f);
                //fprintf(stderr, "player1 speed affected %f \n", m_player1.get_speed());
                itmud->set_affected(1, true);
            }
        }
        if (collided2) {
            bool beingaffected = false;
            for (auto& mud_collected : m_mud_collected)
            {
                if (mud_collected.is_affected(2))
                    beingaffected = true;
            }
            if (!beingaffected)
            {
                m_player2.set_originalspeed(m_player2.get_speed());
                //fprintf(stderr, "player2 speed %f \n", m_player2.get_speed());
                m_player2.set_speed(m_player2.get_speed()-(100.f*m_player2.get_mass()));
                if (m_player2.get_speed() <= 0.f)
                    m_player2.set_speed(10.f);
                //fprintf(stderr, "player2 speed affected %f \n", m_player2.get_speed());
                itmud->set_affected(2, true);
            }
        }
        
        if (!collided1) {
            itmud->set_affected(1, false);
            bool beingaffected = false;
            for (auto& mud_collected : m_mud_collected)
            {
                if (mud_collected.is_affected(1))
                    beingaffected = true;
            }
            if (!beingaffected)
                m_player1.set_speed(m_player1.get_originalspeed());
        }
        if (!collided2) {
            itmud->set_affected(2, false);
            bool beingaffected = false;
            for (auto& mud_collected : m_mud_collected)
            {
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
    
//=================check for MISSILE collision
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
    if (m_player1.collides_with(m_antidote))
        collided = 1;
    if (m_player2.collides_with(m_antidote))
        collided = 2;
    
    if (collided != 0 && m_antidote.belongs_to == 0) {
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
    collided = m_limbsManager.check_collision_with_players(&m_player1, &m_player2, &m_toolboxManager);
//    if (collided != 0) {
//        if (collided <= 2) {
//            m_toolboxManager.addSlot(collided);
//        }
//        else {
//            m_toolboxManager.addSlot(1);
//            m_toolboxManager.addSlot(2);
//        }
//    }
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
    if (droptool_p1 && m_antidote.belongs_to == 1)
    {
        tool_number = 3;
        dropAntidote = true;
    }
    
    if (tool_number == 1) {
        if (!armourInUse_p2 && !droptool_p1)
        {
            immobilize = m_freeze_collected_1.front().use_freeze(2);
            freezeTime = time(0);
        }
        m_player1.set_mass(m_player1.get_mass() - m_freeze_collected_1.begin()->get_mass());
        m_freeze_collected_1.erase(m_freeze_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);

    }
    if (tool_number == 2) {
        if (!droptool_p1)
        {
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
        if (!droptool_p1)
        {
            m_toolboxManager.decreaseSlot(1);
            m_toolboxManager.addSlot(1);
            
            float index = (float) m_toolboxManager.addItem(3, 1);
            if ((int) index != 100) {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
                m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
            }
        }
        else
        {
            std::vector<int> list = m_toolboxManager.getListOfSlot_1();
            for (int i=0; i<list.size(); i++)
            {
                if (list.at(i) == 3)
                    m_toolboxManager.antidotePos = i;
            }
            m_toolboxManager.decreaseSlot(1);
            m_antidote.set_position(m_player1.get_position());
            m_antidote.set_scale({-0.10f * ViewHelper::getRatio(), 0.10f * ViewHelper::getRatio()});
            m_antidote.belongs_to = 0;
        }
        
    }
    if (tool_number == 4) {
//        m_player1.set_mass(m_player1.get_mass() - m_legs_collected_1.begin()->get_mass());
//        m_legs_collected_1.erase(m_legs_collected_1.begin());
        m_limbsManager.decreaseCollectedLegs(1);
        m_toolboxManager.decreaseSlot(1);
        m_player1.increase_speed_legs(- 10);
    }
    if (tool_number == 5) {
        if (!droptool_p1)
        {
            used_bombs.emplace_back(m_bomb_collected_1.front());
            Bomb &use_bomb = used_bombs.back();
            use_bomb.set_position(m_player1.get_position());
            useBomb = true;
            
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
        //if (!armourInUse_p2 && !droptool_p1)
        //tool functionality goes here
        m_player1.set_mass(m_player1.get_mass() - m_missile_collected_1.begin()->get_mass());
        m_missile_collected_1.erase(m_missile_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }
    if (tool_number == 7) {
        if (!droptool_p1)
        {
            armourInUse_p1 = true;
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
    int legcount = 0;
    int bombcount = 0;
    int armourcount = 0;
    int missilecount = 0;
    int antidotePos = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end();++it) {
        
        if (m_toolboxManager.antidotePos == antidotePos && droppedAntidote)
        {
            droppedAntidote = false;
            m_toolboxManager.antidotePos = 0;
        }
        if (*it == 1) {
            if (!droppedAntidote)
            {
                Ice &freeze = m_freeze_collected_1.at(freezecount);
                freeze.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            freezecount++;
        }
        if (*it == 2) {
            if (!droppedAntidote)
            {
                Water &water = m_water_collected_1.at(watercount);
                water.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            watercount++;
        }
        if (*it == 3) {
            if (!droppedAntidote)
            {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
        }
        if (*it == 4) {
            if (!droppedAntidote)
            {
                m_limbsManager.shiftCollectedLegs(1, &m_toolboxManager, index, legcount);
            }
            legcount++;
        }
        if (*it == 5) {
            if (!droppedAntidote)
            {
                Bomb &bomb = m_bomb_collected_1.at(bombcount);
                bomb.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            bombcount++;
        }
        if (*it == 6) {
            if (!droppedAntidote)
            {
                Missile &missile = m_missile_collected_1.at(missilecount);
                missile.set_position(m_toolboxManager.new_tool_position(index, 1));
            }
            missilecount++;
        }
        if (*it == 7) {
            if (!droppedAntidote)
            {
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
    if (droptool_p2 && m_antidote.belongs_to == 2)
    {
        tool_number = 3;
        dropAntidote = true;
    }
    
    if (tool_number == 1) {
        if (!armourInUse_p1 && !droptool_p2)
        {
            immobilize = m_freeze_collected_1.front().use_freeze(1);
            freezeTime = time(0);
        }
        m_player2.set_mass(m_player2.get_mass() - m_freeze_collected_2.begin()->get_mass());
        m_freeze_collected_2.erase(m_freeze_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 2) {
        if (!droptool_p2)
        {
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
        if (!droptool_p2)
        {
            m_toolboxManager.decreaseSlot(2);
            m_toolboxManager.addSlot(2);
            
            float index = (float) m_toolboxManager.addItem(3, 2);
            if ((int) index != 100) {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
                m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
            }
        }
        else
        {
            std::vector<int> list = m_toolboxManager.getListOfSlot_2();
            for (int i=0; i<list.size(); i++)
            {
                if (list.at(i) == 3)
                    m_toolboxManager.antidotePos = i;
            }
            m_toolboxManager.decreaseSlot(2);
            m_antidote.set_position(m_player2.get_position());
            m_antidote.set_scale({-0.10f * ViewHelper::getRatio(), 0.10f * ViewHelper::getRatio()});
            m_antidote.belongs_to = 0;
        }
    }
    if (tool_number == 4) {
        m_limbsManager.decreaseCollectedLegs(2);
        m_toolboxManager.decreaseSlot(2);
        m_player2.increase_speed_legs(- 10);
    }
    if (tool_number == 5) {
        if (!droptool_p2)
        {
            used_bombs.emplace_back(m_bomb_collected_2.front());
            Bomb &use_bomb = used_bombs.back();
            
            use_bomb.set_position(m_player2.get_position());
            useBomb = true;
            
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
        //if (!armourInUse_p1 && !droptool_p2)
        //tool functionality goes here
        m_player2.set_mass(m_player2.get_mass() - m_missile_collected_2.begin()->get_mass());
        m_missile_collected_2.erase(m_missile_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 7) {
        if (!droptool_p2)
        {
            armourInUse_p2 = true;
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
    int legcount = 0;
    int bombcount = 0;
    int armourcount = 0;
    int missilecount = 0;
    int antidotePos = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it) {
        if (m_toolboxManager.antidotePos == antidotePos && droppedAntidote)
        {
            droppedAntidote = false;
            m_toolboxManager.antidotePos = 0;
        }
        
        if (*it == 1) {
            if (!droppedAntidote)
            {
                Ice &freeze = m_freeze_collected_2.at(freezecount);
                freeze.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            freezecount++;
        }
        if (*it == 2) {
            if (!droppedAntidote)
            {
                Water &water = m_water_collected_2.at(watercount);
                water.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            watercount++;
        }
        if (*it == 3) {
            if (!droppedAntidote)
            {
                m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
        }
        if (*it == 4) {
            if (!droppedAntidote)
            {
                m_limbsManager.shiftCollectedLegs(2, &m_toolboxManager, index, legcount);
            }
            legcount++;
        }
        if (*it == 5) {
            if (!droppedAntidote)
            {
                Bomb &bomb = m_bomb_collected_2.at(bombcount);
                bomb.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            bombcount++;
        }
        if (*it == 6) {
            if (!droppedAntidote)
            {
                Missile &missile = m_missile_collected_2.at(missilecount);
                missile.set_position(m_toolboxManager.new_tool_position(index, 2));
            }
            missilecount++;
        }
        if (*it == 7) {
            if (!droppedAntidote)
            {
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
void World::autoExplode() {
    float force_p1 = 0;
    float force_p2 = 0;
    if (!armourInUse_p1)
    {
        force_p1 = used_bombs.front().get_force(m_player1.get_mass(),
                                                m_player1.get_speed(),
                                                m_player1.get_position());
        droptool_p1 = true;
        use_tool_1(m_toolboxManager.useItem(1));
    }
    
    if (!armourInUse_p2)
    {
        force_p2 = used_bombs.front().get_force(m_player2.get_mass(),
                                                m_player2.get_speed(),
                                                m_player2.get_position());
        droptool_p2 = true;
        use_tool_2(m_toolboxManager.useItem(2));
    }
    if (force_p1 > 0) {
        m_player1.set_blowback(true);
        m_player1.set_speed(force_p1);
        m_player1.set_blowbackForce({(m_player1.get_position().x - used_bombs.front().get_position().x),
            (m_player1.get_position().y - used_bombs.front().get_position().y)});
        explosion = true;
    }
    if (force_p2 > 0) {
        m_player2.set_blowback(true);
        m_player2.set_speed(force_p2);
        m_player2.set_blowbackForce({(m_player2.get_position().x - used_bombs.front().get_position().x),
            (m_player2.get_position().y - used_bombs.front().get_position().y)});
        explosion = true;
    }
    
    used_bombs.erase(used_bombs.begin());
    
}

void World::explode() {
    if (m_player1.get_blowback())
        m_player1.set_speed(m_player1.get_speed() * 0.9);
    if (m_player2.get_blowback())
        m_player2.set_speed(m_player2.get_speed() * 0.9);
    
    if (m_player1.get_speed() <= 3 || m_player2.get_speed() <= 3) {
        m_player1.set_blowback(false);
        explosion = false;
        m_player1.set_speed(m_player1.get_speed_legs());
        m_player2.set_blowback(false);
        explosion = false;
        m_player2.set_speed(m_player2.get_speed_legs());
    }
}

void World::use_bomb(float ms) {
    std::vector<Bomb>::iterator itbomb;
    std::vector<Bomb>::iterator checkbomb;
    for (itbomb = used_bombs.begin(); itbomb != used_bombs.end();) {
        itbomb->set_speed({itbomb->get_speed().x * (0.997f), itbomb->get_speed().y * (0.997f)});
        
        if (std::fabs(itbomb->get_speed().x) <= 50 && std::fabs(itbomb->get_speed().y) <= 50)
        {
            itbomb->set_speed({0.f, 0.f});
            autoExplode();
        }
        else
        {
            itbomb->move({itbomb->get_speed().x * (ms / 1000),
                itbomb->get_speed().y * (ms / 1000)});
            itbomb->checkBoundaryCollision(1100, 500, ms);
            
            for (checkbomb = used_bombs.begin(); checkbomb != used_bombs.end() - 1; ++checkbomb) {
                if (checkbomb != itbomb) {
                    if (itbomb->collides_with(*checkbomb))
                        itbomb->checkCollision(*checkbomb, ms);
                }
            }
            
            ++itbomb;
        }
        
    }
    
}

void World::populateMapCollisionPoints() {

mapCollisionPoints.push_back({ 223.867f * ViewHelper::getRatio(), 247.844f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 253.789f * ViewHelper::getRatio(), 207.f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 222.035f * ViewHelper::getRatio(), 135.445f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 280.605f * ViewHelper::getRatio(), 91.4688f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 399.387f * ViewHelper::getRatio(), 73.6523f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 383.434f * ViewHelper::getRatio(), 54.6992f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 450.113f * ViewHelper::getRatio(), 37.8789f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 574.777f * ViewHelper::getRatio(), 36.1602f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 638.223f * ViewHelper::getRatio(), 52.457f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 802.496f * ViewHelper::getRatio(), 36.4766f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 947.973f * ViewHelper::getRatio(), 41.2305f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 990.297f * ViewHelper::getRatio(), 57.4141f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 1048.98f * ViewHelper::getRatio(), 48.7969f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 1002.9f * ViewHelper::getRatio(), 97.2695f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 983.055f * ViewHelper::getRatio(), 154.824f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 995.055f * ViewHelper::getRatio(), 183.164f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 970.074f * ViewHelper::getRatio(), 200.82f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 930.824f * ViewHelper::getRatio(), 272.875f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 979.504f * ViewHelper::getRatio(), 296.066f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 1097.2f * ViewHelper::getRatio(), 329.453f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 1103.01f * ViewHelper::getRatio(), 379.234f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 1001.41f * ViewHelper::getRatio(), 426.199f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 960.559f * ViewHelper::getRatio(), 495.484f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 859.465f * ViewHelper::getRatio(), 491.738f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 739.609f * ViewHelper::getRatio(), 510.684f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 613.824f * ViewHelper::getRatio(), 583.027f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 329.875f * ViewHelper::getRatio(), 479.305f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 307.199f * ViewHelper::getRatio(), 414.918f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 271.188f * ViewHelper::getRatio(), 407.184f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 256.871f * ViewHelper::getRatio(), 368.508f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 195.324f * ViewHelper::getRatio(), 338.215f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 164.711f * ViewHelper::getRatio(), 316.941f * ViewHelper::getRatio()});
mapCollisionPoints.push_back({ 187.73f * ViewHelper::getRatio(), 254.641f * ViewHelper::getRatio()});

}



