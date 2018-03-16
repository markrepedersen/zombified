// Header
#include "world.hpp"
#include "Physics.h"

// stlib
#include <sstream>

namespace {
//insert constants such as max_zombies,
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
    // Seeding rng with random device
    m_rng = std::default_random_engine(std::random_device()());
}

World::~World() {
}

// World initialization
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

    //set game screen to resolution ratio
    ViewHelper::getInstance(m_window);

    /*!
     * Mark's playground so I remember where I put this stuff
     */
    auto tileX = (unsigned) (screen.x * ViewHelper::getRatio()/100);
    auto tileY = (unsigned) (screen.y * ViewHelper::getRatio()/100);
    mapGrid = new MapGrid(tileX, tileY);
    /*!
     * End of Mark's playground
     */

    populateMapCollisionPoints();


    game_started = false;
    game_over = false;
    return m_button.init();
}

// Releases all the associated resources
void World::destroy() {
    m_worldtexture.destroy();
    m_toolboxManager.destroy();
    m_limbsManager.destroy();
    m_player1.destroy();
    m_player2.destroy();
    m_zombie.destroy();
    for (auto &freeze : m_freeze)
    m_tree.destroy();
    for (auto& legs : m_legs)
     	legs.destroy();
    for (auto& arms : m_arms)
     	arms.destroy();
    for (auto& freeze : m_freeze)
        freeze.destroy();
    for (auto &water : m_water)
        water.destroy();
    for (auto& missile : m_missile)
        missile.destroy();
    for (auto& armour : m_armour)
        armour.destroy();
    for (auto& bomb : m_bomb)
        bomb.destroy();
    for (auto &freeze_collected : m_freeze_collected_1)
        freeze_collected.destroy();
    for (auto &water_collected : m_water_collected_1)
        water_collected.destroy();
    for (auto &legs_collected : m_legs_collected_1)
        legs_collected.destroy();
    for (auto &freeze_collected : m_freeze_collected_2)
        freeze_collected.destroy();
    for (auto &water_collected : m_water_collected_2)
        water_collected.destroy();

    for (auto &legs_collected : m_legs_collected_2)
        legs_collected.destroy();

    for (auto& bomb_used : used_bombs)
        bomb_used.destroy();

    m_legs.clear();
    m_arms.clear();
    m_freeze.clear();
    m_water.clear();
    m_missile.clear();
    m_armour.clear();
    m_bomb.clear();
    m_freeze_collected_1.clear();
    m_freeze_collected_2.clear();
    m_water_collected_1.clear();
    m_water_collected_2.clear();
    m_legs_collected_1.clear();
    m_legs_collected_2.clear();

    //glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms) {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    vec2 screen = {(float) w, (float) h};

    if (!game_started) {
        if (m_button.is_clicked()) {
            game_started = true;
            m_button.destroy();


            // initialize everything for the main game world once the button is pressed
            explosion = false;
            m_min = 2;
            m_sec = 0;
            timeDelay = 5;
            start = time(0);
            immobilize = 0;
            m_worldtexture.init(screen);
            m_toolboxManager.init({screen.x, screen.y});
            m_limbsManager.init({screen.x, screen.y});
            m_player1.init(screen) && m_player2.init(screen) && m_antidote.init(screen);
            //m_tree.init(screen);
            //t_bomb.init();
            //t2_bomb.init();
            //t2_bomb.set_position({500.f, 500.f});
            useBomb = false;
            m_player1.init(screen) && m_player2.init(screen)&& m_antidote.init(screen);
        }
    }

    if (game_started) {
        m_player1.update(elapsed_ms);
        m_player2.update(elapsed_ms);

        random_spawn(elapsed_ms, screen);

        if ((int) difftime(time(0), start) == timeDelay)
            timer_update();

        // Next milestone this will be handled by the collision
        check_add_tools(screen);
        m_limbsManager.computePaths(elapsed_ms, *mapGrid);

        if (explosion)
            explode();

        if (useBomb)
            use_bomb(elapsed_ms);

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

    // Clearing backbuffer
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

    mat3 projection_2D{{sx,  0.f, 0.f},
                       {0.f, sy,  0.f},
                       {tx,  ty,  1.f}};

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
        toolManager.draw_tools(projection_2D);

        //TODO: Drawing entities
        for (auto &arms : m_arms)
            arms.draw(projection_2D);
        for (auto &legs : m_legs)
            legs.draw(projection_2D);
        for (auto &freeze : m_freeze)
            freeze.draw(projection_2D);
        for (auto &water : m_water)
            water.draw(projection_2D);
        for(auto &missile : m_missile)
            missile.draw(projection_2D);
        for(auto &armour : m_armour)
            armour.draw(projection_2D);
        for(auto &bomb : m_bomb)
            bomb.draw(projection_2D);

        for (auto &water_collected : m_water_collected_1)
            water_collected.draw(projection_2D);
        for (auto &water_collected : m_water_collected_2)
            water_collected.draw(projection_2D);

        for (auto &freeze_collected : m_freeze_collected_1)
            freeze_collected.draw(projection_2D);
        for (auto &freeze_collected : m_freeze_collected_2)
            freeze_collected.draw(projection_2D);

        for(auto &missile_collected : m_missile_collected_1)
            missile_collected.draw(projection_2D);
        for(auto &missile_collected : m_missile_collected_2)
            missile_collected.draw(projection_2D);
        for(auto &bomb_collected : m_bomb_collected_1)
            bomb_collected.draw(projection_2D);
        for(auto &bomb_collected : m_bomb_collected_2)
            bomb_collected.draw(projection_2D);
        for(auto &armour_collected : m_armour_collected_1)
            armour_collected.draw(projection_2D);
        for(auto &armour_collected : m_armour_collected_2)
            armour_collected.draw(projection_2D);

        for (auto &leg_collected : m_legs_collected_1)
            leg_collected.draw(projection_2D);
        for (auto &leg_collected : m_legs_collected_2)
            leg_collected.draw(projection_2D);


        for(auto &bomb_used : used_bombs)
            bomb_used.draw(projection_2D);

        m_player1.draw(projection_2D);
        m_player2.draw(projection_2D);
    }

    glfwSwapBuffers(m_window);
}

bool World::is_over() const {
    return glfwWindowShouldClose(m_window);
}

// On key callback
void World::on_key(GLFWwindow *, int key, int, int action, int mod)
{

    // player1 actions
    if (immobilize != 1 && !m_player1.get_blowback())
    {
        if (action == GLFW_PRESS && (key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
            m_player1.set_key(key, true);
        if (action == GLFW_RELEASE && (key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
            m_player1.set_key(key, false);
        if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT_SHIFT)
            use_tool_1(m_toolboxManager.useItem(1));
    }
    if (immobilize == 1 || m_player1.get_blowback()) //player is frozen
    {
        //fprintf(stderr, "frozen");
        m_player1.set_key(GLFW_KEY_UP, false);
        m_player1.set_key(GLFW_KEY_LEFT, false);
        m_player1.set_key(GLFW_KEY_DOWN, false);
        m_player1.set_key(GLFW_KEY_RIGHT, false);
        if((int)difftime( time(0), freezeTime) >= 5)
        {
            immobilize = 0;
            freezeTime = 0;
            //fprintf(stderr, "start");
        }

    }

    // player2 actions
    if (immobilize != 2 && !m_player2.get_blowback())
    {
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
        m_player2.set_key(0, false);
        m_player2.set_key(1, false);
        m_player2.set_key(2, false);
        m_player2.set_key(3, false);
        if((int)difftime( time(0), freezeTime) >= 5)
        {
            immobilize = 0;
            freezeTime = 0;
            //fprintf(stderr, "start");
        }
    }
}


void World::on_mouse_move(GLFWwindow *window, int button, int action, int mod) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
        m_button.clickicon();
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        m_button.click();
        draw();
    }
}

bool World::spawn_freeze() {
    Ice freeze;
    if (freeze.init()) {
        m_freeze.emplace_back(freeze);
        return true;
    }
    fprintf(stderr, "Failed to spawn arm");
    return false;
}

bool World::spawn_missile()
{
    Missile missile;
    if (missile.init())
    {
        m_missile.emplace_back(missile);
        return true;
    }
    fprintf(stderr, "Failed to spawn missile");
    return false;
}

bool World::spawn_armour()
{
    Armour armour;
    if (armour.init())
    {
        m_armour.emplace_back(armour);
        return true;
    }
    fprintf(stderr, "Failed to spawn armour");
    return false;
}

bool World::spawn_bomb()
{
    Bomb bomb;
    if (bomb.init())
    {
        m_bomb.emplace_back(bomb);
        return true;
    }
    fprintf(stderr, "Failed to spawn bomb");
    return false;
}

bool World::spawn_water() {
    Water water;
    if (water.init()) {
        m_water.emplace_back(water);
        return true;
    }
    fprintf(stderr, "Failed to spawn arm");
    return false;
}

bool World::random_spawn(float elapsed_ms, vec2 screen) {
    m_next_arm_spawn -= elapsed_ms;
    m_next_leg_spawn -= elapsed_ms;
    m_next_spawn -= elapsed_ms;

    srand((unsigned) time(0));
    int randNum = rand() % (1000);

    if (randNum % 13 == 0) {
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
    if (randNum % 7 == 0) {
        if (m_freeze.size() <= MAX_FREEZE && m_next_spawn < 0.f) {
            if (!spawn_freeze())
                return false;

            Ice &new_freeze = m_freeze.back();
            new_freeze.set_position({(float) ((rand() % (int) screen.x)),
                                     (float) ((rand() % (int) screen.y))});

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

//==== ADD ITEMS TO TOOLBOX =====
void World::check_add_tools(vec2 screen)
{

    int collided = 0;

//=================check for freeze collision
    //  int freezecount = 0;
    std::vector<Ice>::iterator itf;
    for (itf = m_freeze.begin(); itf != m_freeze.end();)
    {
        if (m_player1.collides_with(*itf))
            collided = 1;
        if (m_player2.collides_with(*itf))
            collided = 2;

        if (collided != 0)
        {
            //fprintf(stderr, "collided");
            float index = (float)m_toolboxManager.addItem(1, collided);
            if ((int)index != 100)
            {
                itf = m_freeze.erase(itf);//m_freeze.begin()+freezecount);
                collect_freeze(*itf, collided, index);
                //fprintf(stderr, "freeze count %d \n", freezecount);
                if(collided == 1)
                {
                    m_player1.set_mass(m_player1.get_mass()+itf->get_mass());
                    //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                }
                if (collided == 2)
                {
                    m_player2.set_mass(m_player2.get_mass()+itf->get_mass());
                    //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                }

            }
            else
                ++itf;

        }
        else
            ++itf;
        // freezecount++;
        collided = 0;
    }

//=================check for water collision
    // int watercount = 0;
    //for (auto& water : m_water)
    std::vector<Water>::iterator itw;
    for (itw = m_water.begin(); itw != m_water.end();)
    {
        if (m_player1.collides_with(*itw))//water))
            collided = 1;
        if (m_player2.collides_with(*itw))//water))
            collided = 2;

        if (collided != 0)
        {
            //fprintf(stderr, "collided");
            float index = (float)m_toolboxManager.addItem(2, collided);
            if ((int)index != 100)
            {
                itw = m_water.erase(itw);//m_water.begin()+watercount);
                collect_water(*itw, collided, index);
                //fprintf(stderr, "water count %d \n", watercount);
                if(collided == 1)
                {
                    m_player1.set_mass(m_player1.get_mass()+itw->get_mass());
                    //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                }
                if (collided == 2)
                {
                    m_player2.set_mass(m_player2.get_mass()+itw->get_mass());
                    //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                }
            }
            else
                //fprintf(stderr, "stuck");
                ++itw;
        }
        else
            ++itw;
        //watercount++;
        collided = 0;
    }

//=================check for bomb collision
    std::vector<Bomb>::iterator itb;
    for (itb = m_bomb.begin(); itb != m_bomb.end();)
    {
        if (m_player1.collides_with(*itb))
            collided = 1;
        if (m_player2.collides_with(*itb))
            collided = 2;

        if (collided != 0)
        {
            //fprintf(stderr, "collided");
            float index = (float)m_toolboxManager.addItem(5, collided);
            if ((int)index != 100)
            {
                itb = m_bomb.erase(itb);//m_water.begin()+watercount);
                collect_bomb(*itb, collided, index);
                //fprintf(stderr, "water count %d \n", watercount);
                if(collided == 1)
                {
                    m_player1.set_mass(m_player1.get_mass()+itb->get_mass());
                    //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                }
                if (collided == 2)
                {
                    m_player2.set_mass(m_player2.get_mass()+itb->get_mass());
                    //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                }
            }
            else
                ++itb;
        }
        else
            ++itb;
        collided = 0;
    }

//=================check for missile collision
    std::vector<Missile>::iterator itm;
    for (itm = m_missile.begin(); itm != m_missile.end();)
    {
        if (m_player1.collides_with(*itm))
            collided = 1;
        if (m_player2.collides_with(*itm))
            collided = 2;

        if (collided != 0)
        {
            //fprintf(stderr, "collided");
            float index = (float)m_toolboxManager.addItem(6, collided);
            if ((int)index != 100)
            {
                itm = m_missile.erase(itm);//m_water.begin()+watercount);
                collect_missile(*itm, collided, index);
                //fprintf(stderr, "water count %d \n", watercount);
                if(collided == 1)
                {
                    m_player1.set_mass(m_player1.get_mass()+itm->get_mass());
                    //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                }
                if (collided == 2)
                {
                    m_player2.set_mass(m_player2.get_mass()+itm->get_mass());
                    //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                }
            }
            else
                ++itm;
        }
        else
            ++itm;
        collided = 0;
    }

//=================check for armour collision
    std::vector<Armour>::iterator ita;
    for (ita = m_armour.begin(); ita != m_armour.end();)
    {
        if (m_player1.collides_with(*ita))
            collided = 1;
        if (m_player2.collides_with(*ita))
            collided = 2;

        if (collided != 0)
        {
            //fprintf(stderr, "collided");
            float index = (float)m_toolboxManager.addItem(7, collided);
            if ((int)index != 100)
            {
                ita = m_armour.erase(ita);//m_water.begin()+watercount);
                collect_armour(*ita, collided, index);
                //fprintf(stderr, "water count %d \n", watercount);
                if(collided == 1)
                {
                    m_player1.set_mass(m_player1.get_mass()+ita->get_mass());
                    //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                }
                if (collided == 2)
                {
                    m_player2.set_mass(m_player2.get_mass()+ita->get_mass());
                    //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                }
            }
            else
                ++ita;
        }
        else
            ++ita;
        collided = 0;
    }

//=================check for leg collision
    std::vector<Legs>::iterator itl;
    for (itl = m_legs.begin(); itl != m_legs.end();)
    {
        if (m_player1.collides_with(*itl)) //arm))
            collided = 1;
        if (m_player2.collides_with(*itl)) //arm))
            collided = 2;


        if (collided != 0)
        {
            float index = (float)m_toolboxManager.addItem(4, collided);
            if ((int)index != 100)
            {
                itl = m_legs.erase(itl);//m_water.begin()+watercount);
                collect_legs(*itl, collided, index);
                if(collided == 1)
                {
                    m_player1.increase_speed_legs(m_player1.get_speed()+20);
                    m_player1.set_mass(itl->get_mass()+m_player1.get_mass());
                    //fprintf(stderr, "massp1 added: %f\n", m_player1.get_mass());
                }
                if (collided == 2)
                {
                    m_player2.increase_speed_legs(m_player2.get_speed()+20);
                    m_player2.set_mass(m_player2.get_mass()+itl->get_mass());
                    //fprintf(stderr, "massp2 added: %f\n", m_player2.get_mass());
                }
                //fprintf(stderr, "legp1 added: %f\n", m_player1.get_speed());
                //fprintf(stderr, "legp2 added: %f\n", m_player2.get_speed());
            }

            else
                ++itl;

        }
        else
            ++itl;
        // armcount++;
        collided = 0;
    }

//=================check for arm collision
    // int armcount = 0;
    // std::vector<int> erase;
    // for (auto& arm : m_arms)
    std::vector<Arms>::iterator it;
    for (it = m_arms.begin(); it != m_arms.end();)
    {
        if (m_player1.collides_with(*it)) //arm))
            collided = 1;
        if (m_player2.collides_with(*it)) //arm))
            collided = 2;


        if (collided != 0)
        {
            //fprintf(stderr, "collided");
            if(m_toolboxManager.addSlot(collided))
            {
                //erase.push_back(armcount);
                it = m_arms.erase(it);
                it->destroy();
                //fprintf(stderr, "arm count %d \n", armcount);

            }
            else
                ++it;

        }
        else
            ++it;
        // armcount++;
        collided = 0;
    }

//=================check for antidote collision

    if (m_player1.collides_with(m_antidote))
        collided = 1;
    if (m_player2.collides_with(m_antidote))
        collided = 2;
    if (collided != 0 && m_antidote.belongs_to == 0)
    {
        float index = (float)m_toolboxManager.addItem(3, collided);
        if ((int)index != 100)
        {
            if (collided == 1)
                m_antidote.belongs_to = 1;
            if (collided == 2)
                m_antidote.belongs_to = 2;
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, collided));
            m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
            //fprintf(stderr, "antidote belongs to %d \n", m_antidote.belongs_to);
        }
        collided = 0;
    }



}

void World::autoExplode()
{

    //explodeTime = time(0);
    //m_player1.set_speed(PLAYER_SPEED_NORMAL);
    //fprintf(stderr, "p1 speed %f\n", m_player1.get_speed());
    float force_p1 = used_bombs.front().get_force(m_player1.get_mass(),
                                  m_player1.get_speed(),
                                  m_player1.get_position());

    float force_p2 = used_bombs.front().get_force(m_player2.get_mass(),
                                   m_player2.get_speed(),
                                   m_player2.get_position());
    //fprintf(stderr, "force1 %f\n", force_p1);
    //fprintf(stderr, "force2 %f\n", force_p2);
    if (force_p1 > 0)
    {
        m_player1.set_blowback(true);
        m_player1.set_speed(force_p1);
        m_player1.set_blowbackForce({(m_player1.get_position().x - used_bombs.front().get_position().x),(m_player1.get_position().y - used_bombs.front().get_position().y)});
        explosion = true;
    }
    if (force_p2 > 0)
    {
        m_player2.set_blowback(true);
        m_player2.set_speed(force_p2);
        m_player2.set_blowbackForce({(m_player2.get_position().x - used_bombs.front().get_position().x),(m_player2.get_position().y - used_bombs.front().get_position().y)});
        explosion = true;
    }

    used_bombs.erase(used_bombs.begin());

}

void World::explode()
{
    //fprintf(stderr, "%f", m_player1.get_speed()*0.8);
    if(m_player1.get_blowback())
        m_player1.set_speed(m_player1.get_speed()*0.9);
    if (m_player2.get_blowback())
        m_player2.set_speed(m_player2.get_speed()*0.9);

    if(m_player1.get_speed() <= 3 || m_player2.get_speed() <= 3)
    {
        m_player1.set_blowback(false);
        explosion = false;
        m_player1.set_speed(m_player1.get_speed_legs());
        m_player2.set_blowback(false);
        explosion = false;
        m_player2.set_speed(m_player2.get_speed_legs());
    }
}

void World::use_bomb(float ms)
{
    //t_bomb.move({t_bomb.get_speed().x *(ms/1000), t_bomb.get_speed().y *(ms/1000)});
    //t_bomb.checkBoundaryCollision(1100, 550, ms);   //600, 600, ms);//1100, 550, ms);

    //t2_bomb.move({t2_bomb.get_speed().x *(ms/1000), t2_bomb.get_speed().y *(ms/1000)});
    //t2_bomb.checkBoundaryCollision(1100, 550, ms);   //600, 600, ms);//1100, 550, ms);

    //t_bomb.checkCollision(t2_bomb, ms);
        //for (auto& bomb_used : used_bombs)
    std::vector<Bomb>::iterator itbomb;
    std::vector<Bomb>::iterator checkbomb;
    for (itbomb = used_bombs.begin(); itbomb != used_bombs.end();)
    {
        itbomb->set_speed({itbomb->get_speed().x*(0.997f), itbomb->get_speed().y*(0.997f)});

        if(std::fabs(itbomb->get_speed().x) <= 10 && std::fabs(itbomb->get_speed().y) <= 10)
        {
            itbomb->set_speed({0.f, 0.f});
            autoExplode();
        }
        else
        {
            itbomb->move({itbomb->get_speed().x * (ms/1000),
                itbomb->get_speed().y * (ms/1000)});
            itbomb->checkBoundaryCollision(1100,500,ms);

            for (checkbomb = used_bombs.begin(); checkbomb != used_bombs.end()-1; ++checkbomb)
            {
                if (checkbomb != itbomb)
                {
                    if (itbomb->collides_with(*checkbomb))
                        itbomb->checkCollision(*checkbomb, ms);
                }
            }

            ++itbomb;
        }

    }

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

void World::collect_bomb(Bomb bomb, int player, float index)
{
    if (player == 1)
    {
        m_bomb_collected_1.emplace_back(bomb);
        Bomb &new_bomb = m_bomb_collected_1.back();
        new_bomb.set_position(m_toolboxManager.new_tool_position(index, player));
        //new_bomb.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
    if (player == 2)
    {
        m_bomb_collected_2.emplace_back(bomb);
        Bomb &new_bomb = m_bomb_collected_2.back();
        new_bomb.set_position(m_toolboxManager.new_tool_position(index, player));
        //new_bomb.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
}

void World::collect_armour(Armour armour, int player, float index)
{
    if (player == 1)
    {
        m_armour_collected_1.emplace_back(armour);
        Armour &new_armour = m_armour_collected_1.back();
        new_armour.set_position(m_toolboxManager.new_tool_position(index, player));
        new_armour.set_scale({-8.5f * ViewHelper::getRatio(), 8.5f * ViewHelper::getRatio()});
    }
    if (player == 2)
    {
        m_armour_collected_2.emplace_back(armour);
        Armour &new_armour = m_armour_collected_2.back();
        new_armour.set_position(m_toolboxManager.new_tool_position(index, player));
        new_armour.set_scale({-8.f * ViewHelper::getRatio(), 8.f * ViewHelper::getRatio()});
    }
}

void World::collect_missile(Missile missile, int player, float index)
{
    if (player == 1)
    {
        m_missile_collected_1.emplace_back(missile);
        Missile &new_missile = m_missile_collected_1.back();
        new_missile.set_position(m_toolboxManager.new_tool_position(index, player));
        new_missile.set_scale({-3.5f * ViewHelper::getRatio(), 3.5f * ViewHelper::getRatio()});
    }
    if (player == 2)
    {
        m_missile_collected_2.emplace_back(missile);
        Missile &new_missile = m_missile_collected_2.back();
        new_missile.set_position(m_toolboxManager.new_tool_position(index, player));
        new_missile.set_scale({-3.5f * ViewHelper::getRatio(), 3.5f * ViewHelper::getRatio()});
    }
}

void World::collect_legs(Legs leg, int player, float index)
{
    if (player == 1)
    {
        m_legs_collected_1.emplace_back(leg);
        Legs &new_leg = m_legs_collected_1.back();
        new_leg.set_position(m_toolboxManager.new_tool_position(index, player));
        //new_leg.set_scale({-0.25f * ViewHelper::getRatio(), 0.25f * ViewHelper::getRatio()});
    }
    if (player == 2)
    {
        m_legs_collected_2.emplace_back(leg);
        Legs &new_leg = m_legs_collected_2.back();
        new_leg.set_position(m_toolboxManager.new_tool_position(index, player));
        //new_leg.set_scale({-0.25f * ViewHelper::getRatio(), 0.25f * ViewHelper::getRatio()});
    }
}


void World::use_tool_1(int tool_number)
{
    if (tool_number == 1)
    {
        immobilize = m_freeze_collected_1.front().use_freeze(2);
        freezeTime = time(0);
        m_player1.set_mass(m_player1.get_mass()-m_freeze_collected_1.begin()->get_mass());
        //fprintf(stderr, "massp1 decreased: %f\n", m_player1.get_mass());
        m_freeze_collected_1.erase(m_freeze_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);

    }
    if (tool_number == 2)
    {
        m_player1.set_mass(m_player1.get_mass()-m_water_collected_1.begin()->get_mass());
        m_water_collected_1.erase(m_water_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }

    //antidote is the first tool, move it to the back
    if (tool_number == 3)
    {
        m_toolboxManager.decreaseSlot(1);
        m_toolboxManager.addSlot(1);

        float index = (float)m_toolboxManager.addItem(3, 1);
        if ((int)index != 100)
        {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
            m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
        }
    }
    if (tool_number == 4)
    {
        m_player1.set_mass(m_player1.get_mass()-m_legs_collected_1.begin()->get_mass());
        m_legs_collected_1.erase(m_legs_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
        m_player1.increase_speed_legs(m_player1.get_speed()-20);
        //fprintf(stderr, "legp1 deleted: %f\n", m_player1.get_speed());
    }
    if (tool_number == 5)
    {
        used_bombs.emplace_back(m_bomb_collected_1.front());
        Bomb &use_bomb = used_bombs.back();
        m_bomb_collected_1.erase(m_bomb_collected_1.begin());

        m_player1.set_mass(m_player1.get_mass()- use_bomb.get_mass());
        use_bomb.set_position(m_player1.get_position());
        useBomb = true;
        use_bomb.set_speed({200, 100});

        m_toolboxManager.decreaseSlot(1);
    }
    if (tool_number == 6)
    {
        m_player1.set_mass(m_player1.get_mass()-m_missile_collected_1.begin()->get_mass());
        m_missile_collected_1.erase(m_missile_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }
    if (tool_number == 7)
    {
        m_player1.set_mass(m_player1.get_mass()-m_armour_collected_1.begin()->get_mass());
        m_armour_collected_1.erase(m_armour_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }

    shift_1();
}

void World::shift_1() {
    std::vector<int> list = m_toolboxManager.getListOfSlot_1();
    std::vector<int>::iterator it;
    int freezecount = 0;
    int watercount = 0;
    int legcount = 0;
    int bombcount = 0;
    int armourcount = 0;
    int missilecount = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it) {
        if (*it == 1) {
            Ice &freeze = m_freeze_collected_1.at(freezecount);
            freeze.set_position(m_toolboxManager.new_tool_position(index, 1));
            freezecount++;
        }
        if (*it == 2) {
            Water &water = m_water_collected_1.at(watercount);
            water.set_position(m_toolboxManager.new_tool_position(index, 1));
            watercount++;
        }
        if (*it == 3) {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
        }
        if (*it == 4)
        {
            Legs& legs = m_legs_collected_1.at(legcount);
            legs.set_position(m_toolboxManager.new_tool_position(index, 1));
            legcount++;
        }
        if (*it == 5)
        {
            Bomb& bomb = m_bomb_collected_1.at(bombcount);
            bomb.set_position(m_toolboxManager.new_tool_position(index, 1));
            bombcount++;
        }
        if (*it == 6)
        {
            Missile& missile = m_missile_collected_1.at(missilecount);
            missile.set_position(m_toolboxManager.new_tool_position(index, 1));
            missilecount++;
        }
        if (*it == 7)
        {
            Armour& armour = m_armour_collected_1.at(armourcount);
            armour.set_position(m_toolboxManager.new_tool_position(index, 1));
            armourcount++;
        }
        index++;
    }
}

void World::use_tool_2(int tool_number)
{
    if (tool_number == 1)
    {
        immobilize = m_freeze_collected_1.front().use_freeze(1);
        freezeTime = time(0);
        m_player2.set_mass(m_player2.get_mass()-m_freeze_collected_2.begin()->get_mass());
        //fprintf(stderr, "massp1 decreased: %f\n", m_player1.get_mass());
        m_freeze_collected_2.erase(m_freeze_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 2)
    {
        m_player2.set_mass(m_player2.get_mass()-m_water_collected_2.begin()->get_mass());
        //fprintf(stderr, "massp1 decreased: %f\n", m_player1.get_mass());
        m_water_collected_2.erase(m_water_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }

    //antidote is the first tool, move it to the back
    if (tool_number == 3)
    {
        m_toolboxManager.decreaseSlot(2);
        m_toolboxManager.addSlot(2);

        float index = (float)m_toolboxManager.addItem(3, 2);
        if ((int)index != 100)
        {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
            m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
        }
    }
    if (tool_number == 4)
    {
        m_player2.set_mass(m_player2.get_mass()-m_legs_collected_2.begin()->get_mass());
        //fprintf(stderr, "massp1 decreased: %f\n", m_player1.get_mass());
        m_legs_collected_2.erase(m_legs_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
        m_player2.increase_speed_legs(m_player2.get_speed()-20);
        //fprintf(stderr, "legp2 delete: %f\n", m_player2.get_speed());
    }
    if (tool_number == 5)
    {
        used_bombs.emplace_back(m_bomb_collected_2.front());
        Bomb &use_bomb = used_bombs.back();
        m_bomb_collected_2.erase(m_bomb_collected_2.begin());

        m_player2.set_mass(m_player2.get_mass()- use_bomb.get_mass());
        use_bomb.set_position(m_player2.get_position());
        useBomb = true;
        use_bomb.set_speed({200, 100});

        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 6)
    {
        m_player2.set_mass(m_player2.get_mass()-m_missile_collected_2.begin()->get_mass());
        m_missile_collected_2.erase(m_missile_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 7)
    {
        m_player2.set_mass(m_player2.get_mass()-m_armour_collected_2.begin()->get_mass());
        m_armour_collected_2.erase(m_armour_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    shift_2();

}

void World::shift_2() {
    std::vector<int> list = m_toolboxManager.getListOfSlot_2();
    std::vector<int>::iterator it;
    int freezecount = 0;
    int watercount = 0;
    int legcount = 0;
    int bombcount = 0;
    int armourcount = 0;
    int missilecount = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it) {
        if (*it == 1) {
            Ice &freeze = m_freeze_collected_2.at(freezecount);
            freeze.set_position(m_toolboxManager.new_tool_position(index, 2));
            freezecount++;
        }
        if (*it == 2) {
            Water &water = m_water_collected_2.at(watercount);
            water.set_position(m_toolboxManager.new_tool_position(index, 2));
            watercount++;
        }
        if (*it == 3) {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
        }
        if (*it == 4)
        {
            Legs& legs = m_legs_collected_2.at(legcount);
            legs.set_position(m_toolboxManager.new_tool_position(index, 2));
            legcount++;
        }
        if (*it == 5)
        {
            Bomb& bomb = m_bomb_collected_2.at(bombcount);
            bomb.set_position(m_toolboxManager.new_tool_position(index, 2));
            bombcount++;
        }
        if (*it == 6)
        {
            Missile& missile = m_missile_collected_2.at(missilecount);
            missile.set_position(m_toolboxManager.new_tool_position(index, 2));
            missilecount++;
        }
        if (*it == 7)
        {
            Armour& armour = m_armour_collected_2.at(armourcount);
            armour.set_position(m_toolboxManager.new_tool_position(index, 2));
            armourcount++;
        }
        index++;
    }
}

void World::populateMapCollisionPoints()
{
    mapCollisionPoints.push_back({204 * ViewHelper::getRatio() , 103 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({274 * ViewHelper::getRatio() ,85 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({401 * ViewHelper::getRatio() ,81 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({375 * ViewHelper::getRatio() ,62 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({460 * ViewHelper::getRatio() ,46 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({572 * ViewHelper::getRatio() ,48 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({634 * ViewHelper::getRatio() ,67 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({728 * ViewHelper::getRatio() ,61 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({805 * ViewHelper::getRatio() ,48 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({901 * ViewHelper::getRatio() ,55 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({948 * ViewHelper::getRatio() ,54 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({983 * ViewHelper::getRatio() ,72 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({1046 * ViewHelper::getRatio() ,133 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1068 * ViewHelper::getRatio() ,170 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1018 * ViewHelper::getRatio() ,192 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1010 * ViewHelper::getRatio() ,246 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({1054 * ViewHelper::getRatio() ,272 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({969 * ViewHelper::getRatio() ,274 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({958 * ViewHelper::getRatio() ,251 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1004 * ViewHelper::getRatio() ,227 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({991 * ViewHelper::getRatio() ,227 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1008 * ViewHelper::getRatio() ,188 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1026 * ViewHelper::getRatio() ,216 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1115 * ViewHelper::getRatio() ,213 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({1164 * ViewHelper::getRatio() ,256 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1195 * ViewHelper::getRatio() ,291 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1243 * ViewHelper::getRatio() ,312 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1203 * ViewHelper::getRatio() ,321 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({1195 * ViewHelper::getRatio() ,305 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1125 * ViewHelper::getRatio() ,302 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1045 * ViewHelper::getRatio() ,323 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({964 * ViewHelper::getRatio() ,349 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({981 * ViewHelper::getRatio() ,419 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1103 * ViewHelper::getRatio() ,474 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({1130 * ViewHelper::getRatio() ,535 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({997 * ViewHelper::getRatio() ,530 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({982 * ViewHelper::getRatio() ,548 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({841 * ViewHelper::getRatio() ,618 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({732 * ViewHelper::getRatio() ,511 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({603 * ViewHelper::getRatio() ,434 * ViewHelper::getRatio() });
        mapCollisionPoints.push_back({305 * ViewHelper::getRatio() ,446 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({285 * ViewHelper::getRatio() ,394 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({255 * ViewHelper::getRatio() ,367 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({232 * ViewHelper::getRatio() ,319 * ViewHelper::getRatio() });    mapCollisionPoints.push_back({204 * ViewHelper::getRatio() ,66 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({173 * ViewHelper::getRatio() ,251 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({136 * ViewHelper::getRatio() ,319 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({173 * ViewHelper::getRatio() ,251 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({203 * ViewHelper::getRatio() ,241 * ViewHelper::getRatio() });
    mapCollisionPoints.push_back({164 * ViewHelper::getRatio() ,167 * ViewHelper::getRatio() });

}



