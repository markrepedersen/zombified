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
const size_t MAX_ARMS = 5;
const size_t MAX_LEGS = 5;
const size_t MAX_FREEZE = 5;
const size_t MAX_WATER = 5;
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

    // Playing background music undefinitely
    //Mix_PlayMusic(m_background_music, -1);

    //fprintf(stderr, "Loaded music");



      //set game screen to resolution ratio
    ViewHelper* vh = ViewHelper::getInstance(m_window);

    game_started = false;
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
    //m_toolboxManager.destroy();
    //m_player1.destroy();
    //m_player2.destroy();
    //m_water.destroy();
   // m_freeze.destroy();
    for (auto& legs : m_legs)
     	legs.destroy();
    for (auto& arms : m_arms)
     	arms.destroy();
    for (auto& freeze : m_freeze)
        freeze.destroy();

    glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    vec2 screen = {(float)w, (float)h};

    //TODO: check for
    //player to antidote collisions?
    //limb to limb collision?
    //zombie to antidote collision?
    //zombie to player collision?
    //item to player collision?
    
    // start page
    while (!game_started)
    {
        if(m_button.is_clicked())
        {
            game_started = true;
            m_button.destroy();
            
            // initialize everything for the main game world once the button is pressed
            m_min = 6;
            m_sec = 0;
            m_counter = 5;
            start = time(0);
            m_worldtexture.init(screen);
            m_toolboxManager.init({screen.x, screen.y+200});
            m_player1.init(screen) && m_player2.init(screen)&& m_antidote.init(screen);
        }
        else
        {
        // Processes system messages, if this wasn't present the window would become unresponsive
            glfwPollEvents();
            return true;
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
        
        if ((int)difftime( time(0), start) == m_counter)
            timer_update();
        
        // Next milestone this will be handled by the collision
        check_add_tools(screen);
        
        return true;
    }
    return true;
}

void World::timer_update()
{

//TODO use time elapsed instead?
    if (m_sec == 0)
    {
        m_sec = 59;
        m_min -=1;
        m_counter++;
    }
    else
    {
        m_sec -= 1;
        m_counter ++;
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
        for (auto &freeze : m_freeze_collected)
            freeze.draw(projection_2D);
        for (auto &freeze : m_freeze)
            freeze.draw(projection_2D);
        for (auto &water : m_water)
            water.draw(projection_2D);
        for (auto &water : m_water_collected)
            water.draw(projection_2D);
        m_player1.draw(projection_2D);
        m_player2.draw(projection_2D);
        
        // TODO: will need to spawn random arms and legs
        //m_arms.draw(projection_2D);
        //m_legs.draw(projection_2D);
        //m_water.draw(projection_2D);
        //m_freeze.draw(projection_2D);
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

    if (action == GLFW_PRESS && (key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
        m_player1.set_key(key, true);
    if (action == GLFW_RELEASE && (key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
        m_player1.set_key(key, false);
    
    if (action == GLFW_PRESS && key == GLFW_KEY_W)
        m_player2.set_key(0, true);
    if (action == GLFW_RELEASE && key == GLFW_KEY_W)
        m_player2.set_key(0, false);
    if (action == GLFW_PRESS && key == GLFW_KEY_A)
        m_player2.set_key(1, true);
    if (action == GLFW_RELEASE && key == GLFW_KEY_A)
        m_player2.set_key(1, false);
    if (action == GLFW_PRESS && key == GLFW_KEY_S)
        m_player2.set_key(2, true);
    if (action == GLFW_RELEASE && key == GLFW_KEY_S)
        m_player2.set_key(2, false);
    if (action == GLFW_PRESS && key == GLFW_KEY_D)
        m_player2.set_key(3, true);
    if (action == GLFW_RELEASE && key == GLFW_KEY_D)
        m_player2.set_key(3, false);

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

//======== SPAWNS =======
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

//==== add items to toolbox =====
void World::check_add_tools(vec2 screen)
{

    int collided = 0;
    
//=================check for water freeze
    int freezecount = 0;
    for (auto& freeze : m_freeze)
    {
        if (m_player1.collides_with(freeze))
            collided = 1;
        if (m_player2.collides_with(freeze))
            collided = 2;
        
        if (collided != 0)
        {
            float index = (float)m_toolboxManager.addItem(1, collided);
            if ((int)index != 100)
            {
                Freeze newfreeze;
                newfreeze.init();
                m_freeze_collected.emplace_back(newfreeze);
                Freeze &new_freeze = m_freeze_collected.back();
                new_freeze.set_position(m_toolboxManager.new_tool_position(index, collided));
                new_freeze.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
                
                m_freeze.erase(m_freeze.begin()+freezecount);
                freeze.destroy();
            }
        
        }
        freezecount++;
        collided = 0;
    }

//=================check for water collision
    int watercount = 0;
    for (auto& water : m_water)
    {
        if (m_player1.collides_with(water))
            collided = 1;
        if (m_player2.collides_with(water))
            collided = 2;
        
        if (collided != 0)
        {
            float index = (float)m_toolboxManager.addItem(2, collided);
            if ((int)index != 100)
            {
                Water newwater;
                newwater.init();
                m_water_collected.emplace_back(newwater);
                Water &new_water = m_water_collected.back();
                new_water.set_position(m_toolboxManager.new_tool_position(index, collided));
                new_water.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
                
                m_water.erase(m_water.begin()+watercount);
                water.destroy();
            }
        }
        watercount++;
        collided = 0;
    }
    
//=================check for arm collision
    int armcount = 0;
    for (auto& arm : m_arms)
    {
        if (m_player1.collides_with(arm))
            collided = 1;
        if (m_player2.collides_with(arm))
            collided = 2;
        
        if (collided != 0)
        {
            if(m_toolboxManager.addSlot(collided))
            {
                m_arms.erase(m_arms.begin()+armcount);
                arm.destroy();
            }
        }
        armcount++;
        collided = 0;
    }
    
//=================check for antidote collision
    if (m_player1.collides_with(m_antidote))
        collided = 1;
    if (m_player2.collides_with(m_antidote))
        collided = 2;
    if (collided != 0)
    {
        float index = (float)m_toolboxManager.addItem(3, collided);
        if ((int)index != 100)
        {
            m_antidote.set_position(m_toolboxManager.new_tool_position(index, collided));
            m_antidote.set_scale({-0.08f * ViewHelper::getRatio(), 0.08f * ViewHelper::getRatio()});
            
        }
        collided = 0;
    }
    
    
}

bool World::random_spawn(float elapsed_ms, vec2 screen)
{
    m_next_arm_spawn -= elapsed_ms;
    m_next_leg_spawn -= elapsed_ms;
    m_next_spawn -= elapsed_ms;
    
    srand((unsigned)time(0));
    int randNum = rand() % (1000);
    
    if (randNum % 3 == 0)
    {
        if (m_arms.size() <= MAX_ARMS && m_next_arm_spawn < 0.f)
        {
            if (!spawn_arms())
                return false;
            
            Arms &new_arm = m_arms.back();
            
            // Setting random initial position
            //TODO: should make sure they spawn a certain distance away from each other and check collision with wall
            //TODO: should we scale this with ViewHelper::getRatio();???
            //srand((unsigned)time(0));
            new_arm.set_position({(float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y))});
            
            // Next spawn
            //srand((unsigned)time(0));
            m_next_arm_spawn = (ARM_DELAY_MS / 2) + rand() % (1000);
        }
    }
    
    if (randNum % 8 == 0)
    {
        if (m_legs.size() <= MAX_LEGS && m_next_leg_spawn < 0.f)
        {
            if (!spawn_legs())
                return false;
            
            Legs &new_leg = m_legs.back();
            
            // Setting random initial position
            //TODO: should make sure they spawn a certain distance away from each other and check collision with wall
            //srand((unsigned)time(0));
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

