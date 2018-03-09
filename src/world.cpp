// Header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <math.h>
#include <time.h>

// Same as static in c, local to compilation unit
namespace
{

//TODO
//insert constants such as max_zombies,
const size_t MAX_ARMS = 3;
const size_t MAX_LEGS = 3;
const size_t MAX_FREEZE = 2;
const size_t MAX_MISSILE = 4;
const size_t MAX_WATER = 2;
const size_t ARM_DELAY_MS = 1000;
const size_t LEG_DELAY_MS = 1000;
const size_t DELAY_MS = 1000;

namespace
{
void glfw_err_cb(int error, const char *desc)
{
    fprintf(stderr, "%d: %s", error, desc);
}
}
}


World::World() :
m_next_arm_spawn(rand()%(1000)+500),
m_next_leg_spawn(rand()%(1000)+500),
m_next_spawn(rand()%(1000)+600)


{
    // Seeding rng with random device
    m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{
}

// World initialization
bool World::init(vec2 screen)
{
    //-------------------------------------------------------------------------
    // GLFW / OGL Initialization
    // Core Opengl 3.
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit())
    {
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
    m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "Zombified", nullptr, nullptr);
    if (m_window == nullptr)
        return false;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    // Load OpenGL function pointers
    gl3w_init();

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(m_window, this);
    auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) { ((World *)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
    //auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) { ((World *)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
    auto mouse_button_callback = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1, _2); };
    
    glfwSetKeyCallback(m_window, key_redirect);
    //glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);

    //-------------------------------------------------------------------------
    // Loading music and sounds

//    if (SDL_Init(SDL_INIT_AUDIO) < 0)
//    {
//        fprintf(stderr, "Failed to initialize SDL Audio");
//        return false;
//    }
//    
//    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
//    {
//        fprintf(stderr, "Failed to open audio device");
//        return false;
//    }
    
    
    //TODO: set up music files here
    // m_background_music = Mix_LoadMUS(audio_path("music.wav"));
   

    //TODO: check if sounds were loaded
    // if (m_background_music == nullptr || m_salmon_dead_sound == nullptr || m_salmon_eat_sound == nullptr)
    // {
    // 	fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
    // 	return false;
    // }

    // Playing background music indefinitely
    //Mix_PlayMusic(m_background_music, -1);

    //fprintf(stderr, "Loaded music");
    mapGrid = new MapGrid((unsigned) screen.x, (unsigned) screen.y);

      //set game screen to resolution ratio
    ViewHelper* vh = ViewHelper::getInstance(m_window);

    game_started = false;
    game_over = false;
    return m_button.init();
}

// Releases all the associated resources
void World::destroy()
{

    //TODO: free audio files

    // if (m_background_music != nullptr)
    // 	Mix_FreeMusic(m_background_music);
   
    //Mix_CloseAudio();
 
    //TODO: free players, zombies, limbs, any items on the map, walls
    m_worldtexture.destroy();
    m_toolboxManager.destroy();
    m_player1.destroy();
    m_player2.destroy();
    m_zombie.destroy();
    for (auto& legs : m_legs)
     	legs.destroy();
    for (auto& arms : m_arms)
     	arms.destroy();
    for (auto& freeze : m_freeze)
        freeze.destroy();
    for (auto& water : m_water)
        water.destroy();
    for (auto& missile : m_missile)
        missile.destroy();
    for (auto &freeze_collected : m_freeze_collected_1)
        freeze_collected.destroy();
    for (auto &water_collected : m_water_collected_1)
        water_collected.destroy();
    for (auto &freeze_collected : m_freeze_collected_2)
        freeze_collected.destroy();
    for (auto &water_collected : m_water_collected_2)
        water_collected.destroy();
    
    m_legs.clear();
    m_arms.clear();
    m_freeze.clear();
    m_water.clear();
    m_missile.clear();
    m_freeze_collected_1.clear();
    m_freeze_collected_2.clear();
    m_water_collected_1.clear();
    m_water_collected_2.clear();

    //glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    vec2 screen = {(float)w, (float)h};

    //TODO: check for
    //player to antidote collisions?
    //limb to limb collision?
    //zombie to antidote collision?
    //zombie to player collision?
    //item to player collision?
    
    // start page
    if (!game_started)
    {
        if(m_button.is_clicked())
        {
            game_started = true;
            m_button.destroy();
            
            // initialize everything for the main game world once the button is pressed
            m_min = 2;
            m_sec = 0;
            timeDelay = 5;
            start = time(0);
            check_freeze_used = 0;
            m_worldtexture.init(screen);
            m_toolboxManager.init({screen.x, screen.y});
            m_player1.init(screen) && m_player2.init(screen)&& m_antidote.init(screen);
        }
    }
    
    // GAME DOESNT START UNTIL THE BUTTON IS CLICKED
    //fprintf(stderr, "start screen should be destroyed");
    if (game_started)
    {
        //TODO: upating all entities (player, zombies, limbs, items )
        m_player1.update(elapsed_ms);
        m_player2.update(elapsed_ms);
        
        //TOOD: removing all out of screen entities, probably not needed if collision with edges of screen is
        //always checked
        
        //TODO: spawn limbs, items
        random_spawn(elapsed_ms, screen);
        
        if ((int)difftime( time(0), start) == timeDelay)
            timer_update();
        
        // Next milestone this will be handled by the collision
        check_add_tools(screen);
        computePaths(elapsed_ms);
        
        return true;
    }
    return true;
}

void World::timer_update()
{

//TODO use time elapsed instead?
    if (m_min == 0 && m_sec == 0)
    {
        game_over = true;
        fprintf(stderr, "winner is %d \n", m_antidote.belongs_to);
        destroy();
        m_button.init();
        game_started = false;
        m_min = 0;
        m_sec = 0;
    }
    else if (m_sec == 0)
    {
        m_sec = 59;
        m_min -= 1;
        timeDelay++;
    }
    else
    {
        m_sec -= 1;
        timeDelay ++;
    }

}

// Render our game world
void World::draw()
{
    // Clearing error buffer
    gl_flush_errors();

    // Getting size of window
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    // Updating window title with points
    std::stringstream title_ss;
    if (m_sec < 10)
        title_ss << "Time remaining " << m_min << ":" << "0" << m_sec;
    else
        title_ss << "Time remaining " << m_min << ":" << m_sec;
    glfwSetWindowTitle(m_window, title_ss.str().c_str());

    // Clearing backbuffer
    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);

    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;        // *-0.5;
    float top = 0.f;         // (float)h * -0.5;
    float right = (float)w;  // *0.5;
    float bottom = (float)h; // *0.5;
    
    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    
    mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
    
    if (!game_started)
    {
        const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
        glClearColor(clear_color[1], clear_color[1], clear_color[1], 1.0);
        glClearDepth(1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        m_button.draw(projection_2D);
    }
    else
    {
        m_worldtexture.draw(projection_2D);
        m_toolboxManager.draw(projection_2D);
        m_antidote.draw(projection_2D);
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
        
        m_player1.draw(projection_2D);
        m_player2.draw(projection_2D);
    }

    // Presenting
    glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over() const
{
    return glfwWindowShouldClose(m_window);
}

// On key callback
void World::on_key(GLFWwindow *, int key, int, int action, int mod)
{

    // player1 actions
    if (check_freeze_used != 1)
    {
        if (action == GLFW_PRESS && (key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
            m_player1.set_key(key, true);
        if (action == GLFW_RELEASE && (key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
            m_player1.set_key(key, false);
        if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT_SHIFT)
            use_tool_1(m_toolboxManager.useItem(1));
    }
    if (check_freeze_used == 1) //player is frozen
    {
        //fprintf(stderr, "frozen");
        m_player1.set_key(GLFW_KEY_UP, false);
        m_player1.set_key(GLFW_KEY_LEFT, false);
        m_player1.set_key(GLFW_KEY_DOWN, false);
        m_player1.set_key(GLFW_KEY_RIGHT, false);
        if((int)difftime( time(0), freezeTime) >= 5)
        {
            check_freeze_used = 0;
            //fprintf(stderr, "start");
        }
    }
    
    // player2 actions
    if (check_freeze_used != 2)
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
    if (check_freeze_used == 2) //player is frozen
    {
        m_player2.set_key(0, false);
        m_player2.set_key(1, false);
        m_player2.set_key(2, false);
        m_player2.set_key(3, false);
        if((int)difftime( time(0), freezeTime) >= 5)
        {
            check_freeze_used = 0;
            //fprintf(stderr, "start");
        }
    }
    

    //	// Resetting game
    //	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
    //	{
    //		int w, h;
    //		glfwGetWindowSize(m_window, &w, &h);
    //		m_salmon.destroy();
    //		m_salmon.init();
    //		//m_turtles.clear();
    //		//m_fish.clear();
    //		m_current_speed = 1.f;
    //	}
    //
    //	// Control the current speed with `<` `>`
    //	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) &&  key == GLFW_KEY_COMMA)
    //		m_current_speed -= 0.1f;
    //	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
    //		m_current_speed += 0.1f;
    //
}

void World::on_mouse_move(GLFWwindow* window, int button, int action, int mod)
{
    // TODO: check that the part clicked is the button
    //      will do when figure out how things are scaled properly ***
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1)
    {
        m_button.clickicon();
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
    {
        m_button.click();
        draw();
    }
}

//======== SPAWNING =======
// Creates a new turtle and if successfull adds it to the list of turtles
bool World::spawn_arms()
{
    Arms arm;
    if (arm.init())
    {
        m_arms.emplace_back(arm);
        return true;
    }
    fprintf(stderr, "Failed to spawn arm");
    return false;
}

bool World::spawn_legs()
{
    Legs leg;
    if (leg.init())
    {
        m_legs.emplace_back(leg);
        return true;
    }
    fprintf(stderr, "Failed to spawn arm");
    return false;
}

bool World::spawn_freeze()
{
    Freeze freeze;
    if (freeze.init())
    {
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

bool World::spawn_water()
{
    Water water;
    if (water.init())
    {
        m_water.emplace_back(water);
        return true;
    }
    fprintf(stderr, "Failed to spawn arm");
    return false;
}

void World::computePaths(float ms) {
    vec2 pos1 = {m_player1.get_position().x, m_player1.get_position().y};
    vec2 pos2 = {m_player2.get_position().x, m_player2.get_position().y};
    for (auto &arm : m_arms) {
        JPS::PathVector path;
        arm.setCurrentTarget(distance(pos1, arm.get_position()) > distance(pos2, arm.get_position()) ? pos2 : pos1);
        vec2 target = arm.getCurrentTarget();

        if (arm.getLastTarget() != target || arm.getLastTarget() == (vec2) {0,0}) {
            JPS::findPath(path,
                          *mapGrid,
                          (unsigned) arm.get_position().x,
                          (unsigned) arm.get_position().y,
                          (unsigned) target.x,
                          (unsigned) target.y,
                          1);
            arm.setCurrentPath(path);
        } else arm.setCurrentPath(arm.getLastPath());
        if (!arm.getCurrentPath().empty()) {
            vec2 nextNode, curNode;
            curNode = nextNode = {std::powf(arm.get_position().x, 2), std::powf(arm.get_position().y,2)};

            for (int i = 0; i < arm.getCurrentPath().size() && curNode <= nextNode; ++i) {
                nextNode = {static_cast<float>(arm.getCurrentPath()[i].x), static_cast<float>(arm.getCurrentPath()[i].y)};
            }
            float step = 20 * (ms / 1000);
            vec2 dir;
            dir.x = arm.getCurrentTarget().x - arm.get_position().x;
            dir.y = arm.getCurrentTarget().y - arm.get_position().y;
            arm.move(scale(step, normalize(dir)));

            arm.setLastPath(arm.getCurrentPath());
            arm.setLastTarget(target);
        }
    }
}

//TODO: should make sure they spawn a certain distance away from each other and not on top of each other
//  check collision with wall
bool World::random_spawn(float elapsed_ms, vec2 screen)
{
    m_next_arm_spawn -= elapsed_ms;
    m_next_leg_spawn -= elapsed_ms;
    m_next_spawn -= elapsed_ms;
    
    srand((unsigned)time(0));
    int randNum = rand() % (1000);
    
    if (randNum % 13 == 0)
    {
        if (m_arms.size() <= MAX_ARMS && m_next_arm_spawn < 0.f)
        {
            if (!spawn_arms())
                return false;
            
            Arms &new_arm = m_arms.back();
            
            // Setting random initial position
            new_arm.set_position({(float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y))});
            
            // Next spawn
            //srand((unsigned)time(0));
            m_next_arm_spawn = (ARM_DELAY_MS / 2) + rand() % (1000);
        }
    }
    
    if (randNum % 19 == 0)
    {
        if (m_legs.size() <= MAX_LEGS && m_next_leg_spawn < 0.f)
        {
            if (!spawn_legs())
                return false;
            
            Legs &new_leg = m_legs.back();
            
            new_leg.set_position({(float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y))});
            
            m_next_leg_spawn = (LEG_DELAY_MS / 2) + rand() % (1000);
        }
    }
    if (randNum % 7 == 0)
    {
        if (m_freeze.size() <= MAX_FREEZE && m_next_spawn < 0.f)
        {
            if (!spawn_freeze())
                return false;
            
            Freeze &new_freeze = m_freeze.back();
            new_freeze.set_position({(float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y))});
            
            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }
    if (randNum % 3 == 0)
    {
        if (m_missile.size() <= MAX_MISSILE && m_next_spawn < 0.f)
        {
            if (!spawn_missile())
                return false;
            
            Missile &new_missile = m_missile.back();
            new_missile.set_position({(float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y))});
            
            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }
    
    if (randNum % 9 == 0)
    {
        if (m_water.size() <= MAX_WATER && m_next_spawn < 0.f)
        {
            if (!spawn_water())
                return false;
            Water &new_water = m_water.back();
            new_water.set_position({(float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y))});
            
            m_next_spawn = (DELAY_MS / 2) + rand() % (1000);
        }
    }
    return true;
}

//==== ADD ITEMS TO TOOLBOX =====
void World::check_add_tools(vec2 screen)
{

    int collided = 0;
    
//=================check for water freeze
  //  int freezecount = 0;
    std::vector<Freeze>::iterator itf;
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
    
//=================check for arm collision
   // int armcount = 0;
    std::vector<int> erase;
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

// =========== COLLECT AND SET TOOLS ===================
void World::collect_freeze(Freeze freeze, int player, float index)
{
    if (player == 1)
    {
        m_freeze_collected_1.emplace_back(freeze);
        Freeze &new_freeze = m_freeze_collected_1.back();
        new_freeze.set_position(m_toolboxManager.new_tool_position(index, player));
        new_freeze.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
    if (player == 2)
    {
        m_freeze_collected_2.emplace_back(freeze);
        Freeze &new_freeze = m_freeze_collected_2.back();
        new_freeze.set_position(m_toolboxManager.new_tool_position(index, player));
        new_freeze.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }

}

void World::collect_water(Water water, int player, float index)
{
    if (player == 1)
    {
        m_water_collected_1.emplace_back(water);
        Water &new_water = m_water_collected_1.back();
        new_water.set_position(m_toolboxManager.new_tool_position(index, player));
        new_water.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
    if (player == 2)
    {
        m_water_collected_2.emplace_back(water);
        Water &new_water = m_water_collected_2.back();
        new_water.set_position(m_toolboxManager.new_tool_position(index, player));
        new_water.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
    }
}


// =========== USE TOOLS and SHIFT the ones unused ===================
void World::use_tool_1(int tool_number)
{
    if (tool_number == 1)
    {
        check_freeze_used = m_freeze_collected_1.front().use_freeze(2);
        freezeTime = time(0);
        m_freeze_collected_1.erase(m_freeze_collected_1.begin());
        m_toolboxManager.decreaseSlot(1);
    }
    if (tool_number == 2)
    {
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
    shift_1();
}

void World::shift_1()
{
    std::vector<int> list = m_toolboxManager.getListOfSlot_1();
    std::vector<int>::iterator it;
    int freezecount = 0;
    int watercount = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it)
    {
        if (*it == 1)
        {
            Freeze& freeze = m_freeze_collected_1.at(freezecount);
            freeze.set_position(m_toolboxManager.new_tool_position(index, 1));
            freezecount++;
        }
        if (*it == 2)
        {
            Water& water = m_water_collected_1.at(watercount);
            water.set_position(m_toolboxManager.new_tool_position(index, 1));
            watercount++;
        }
        if (*it == 3)
        {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, 1));
        }
        index++;
    }
}
    
void World::use_tool_2(int tool_number)
{
    if (tool_number == 1)
    {
        check_freeze_used = m_freeze_collected_1.front().use_freeze(1);
        freezeTime = time(0);
        m_freeze_collected_2.erase(m_freeze_collected_2.begin());
        m_toolboxManager.decreaseSlot(2);
    }
    if (tool_number == 2)
    {
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
    shift_2();
    
}

void World::shift_2()
{
    std::vector<int> list = m_toolboxManager.getListOfSlot_2();
    std::vector<int>::iterator it;
    int freezecount = 0;
    int watercount = 0;
    float index = 0.f;
    for (it = list.begin(); it != list.end(); ++it)
    {
        if (*it == 1)
        {
            Freeze& freeze = m_freeze_collected_2.at(freezecount);
            freeze.set_position(m_toolboxManager.new_tool_position(index, 2));
            freezecount++;
        }
        if (*it == 2)
        {
            Water& water = m_water_collected_2.at(watercount);
            water.set_position(m_toolboxManager.new_tool_position(index, 2));
            watercount++;
        }
        if (*it == 3)
        {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, 2));
        }
        index++;
    }
}



