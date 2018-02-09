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
    // const size_t MAX_TURTLES = 15;
    // const size_t MAX_FISH = 5;
    // const size_t TURTLE_DELAY_MS = 2000;
    // const size_t FISH_DELAY_MS = 5000;
    
    
    //TODO
    //insert constants such as max_zombies,
    const size_t MAX_ARMS = 5;
    const size_t MAX_LEGS = 5;
    const size_t ARM_DELAY_MS = 1000;
    const size_t LEG_DELAY_MS = 1000;

    namespace
    {
        void glfw_err_cb(int error, const char* desc)
        {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

World::World() :
//m_points(0),
m_next_arm_spawn(rand()%(1000)+500),
m_next_leg_spawn(rand()%(1000)+500)
// m_next_turtle_spawn(0.f),
// m_next_fish_spawn(0.f)
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
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
    glfwSetKeyCallback(m_window, key_redirect);
    glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
    
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
    // m_salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
    // m_salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));
    
    //TODO: check if sounds were loaded
    // if (m_background_music == nullptr || m_salmon_dead_sound == nullptr || m_salmon_eat_sound == nullptr)
    // {
    // 	fprintf(stderr, "Failed to load sounds, make sure the data directory is present");
    // 	return false;
    // }
    
    // Playing background music undefinitely
    //Mix_PlayMusic(m_background_music, -1);
    
    //fprintf(stderr, "Loaded music");
    
    //m_current_speed = 1.f;

      //set game screen to resolution ratio
    ViewHelper* vh = ViewHelper::getInstance(m_window);

    m_min = 6;
    m_sec = 0;
    m_counter = 5;
    start = time(0);
    //draw world texture
    m_worldtexture.init(screen);
    //initialize toolbox
    m_toolboxManager.init({screen.x, screen.y});
    
    //TODO return players && walls???
    return m_player1.init(screen) && m_player2.init(screen) && m_water.init() && m_freeze.init() ;
}

// Releases all the associated resources
void World::destroy()
{
    
    //TODO: free audio files
    
    // if (m_background_music != nullptr)
    // 	Mix_FreeMusic(m_background_music);
    // if (m_salmon_dead_sound != nullptr)
    // 	Mix_FreeChunk(m_salmon_dead_sound);
    // if (m_salmon_eat_sound != nullptr)
    // 	Mix_FreeChunk(m_salmon_eat_sound);
    
    //Mix_CloseAudio();
    m_worldtexture.destroy();
    //m_toolboxManager.destroy();
    //m_player1.destroy();
    //m_player2.destroy();
    //m_water.destroy();
    //m_freeze.destroy();
    
    //TODO: free players, zombies, limbs, any items on the map, walls
    // m_salmon.destroy();
    for (auto& legs : m_legs)
     	legs.destroy();
    for (auto& arms : m_arms)
     	arms.destroy();
    // m_turtles.clear();
    // m_fish.clear();
    glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    vec2 screen = { (float)w, (float)h };
    
    //TODO: check for
    //player to antidote collisions?
    //limb to limb collision?
    //zombie to antidote collision?
    //zombie to player collision?
    //item to player collision?
    
    
    // Checking Salmon - Turtle collisions
    // for (const auto& turtle : m_turtles)
    // {
    // 	if (m_salmon.collides_with(turtle))
    // 	{
    // 		if (m_salmon.is_alive())
    // 			Mix_PlayChannel(-1, m_salmon_dead_sound, 0);
    // 		m_salmon.kill();
    // 		break;
    // 	}
    // }
    
    // Checking Salmon - Fish collisions
    // auto fish_it = m_fish.begin();
    // while (fish_it != m_fish.end())
    // {
    // 	if (m_salmon.collides_with(*fish_it))
    // 	{
    // 		fish_it = m_fish.erase(fish_it);
    // 		m_salmon.light_up();
    // 		Mix_PlayChannel(-1, m_salmon_eat_sound, 0);
    // 		++m_points;
    // 	}
    // 	else
    // 		++fish_it;
    // }
    
    //TODO: upating all entities (player, zombies, limbs, items )
    // Updating all entities, making the turtle and fish
    // faster based on current
    // m_salmon.update(elapsed_ms);
    // for (auto& turtle : m_turtles)
    // 	turtle.update(elapsed_ms * m_current_speed);
    // for (auto& fish : m_fish)
    // 	fish.update(elapsed_ms * m_current_speed);
    
    
    //TOOD: removing all out of screen entities, probably not needed if collision with edges of screen is
    //always checked
    
    // // Removing out of screen turtles
    // auto turtle_it = m_turtles.begin();
    // while (turtle_it != m_turtles.end())
    // {
    // 	float w = turtle_it->get_bounding_box().x / 2;
    // 	if (turtle_it->get_position().x + w < 0.f)
    // 	{
    // 		turtle_it = m_turtles.erase(turtle_it);
    // 		continue;
    // 	}
    
    // 	++turtle_it;
    // }
    
    // // Removing out of screen fish
    // fish_it = m_fish.begin();
    // while (fish_it != m_fish.end())
    // {
    // 	float w = fish_it->get_bounding_box().x / 2;
    // 	if (fish_it->get_position().x + w < 0.f)
    // 	{
    // 		fish_it = m_fish.erase(fish_it);
    // 		continue;
    // 	}
    
    // 	++fish_it;
    // }
    
    //TODO: spawn limbs, items
    m_next_arm_spawn -= elapsed_ms;
    m_next_leg_spawn -= elapsed_ms;
    srand((unsigned)time(0));
    int randNum = rand()%(1000);
    
    if (randNum%3 == 0)
    {
        if (m_arms.size() <= MAX_ARMS && m_next_arm_spawn < 0.f)
        {
            if (!spawn_arms())
                return false;
            
            Arms& new_arm = m_arms.back();
            
            // Setting random initial position
            //TODO: should make sure they spawn a certain distance away from each other and check collision with wall
            //TODO: should we scale this with ViewHelper::getRatio();???
            //srand((unsigned)time(0));
            new_arm.set_position({ (float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y)) });
            
            // Next spawn
            //srand((unsigned)time(0));
            m_next_arm_spawn = (ARM_DELAY_MS/2) + rand()%(1000);
        }
    }
    
    if (randNum%8 == 0)
    {
        if (m_legs.size() <= MAX_LEGS && m_next_leg_spawn < 0.f)
        {
            if (!spawn_legs())
                return false;
            
            Legs& new_leg = m_legs.back();
            
            // Setting random initial position
            //TODO: should make sure they spawn a certain distance away from each other and check collision with wall
            //srand((unsigned)time(0));
            new_leg.set_position({ (float)((rand() % (int)screen.x)),
                (float)((rand() % (int)screen.y)) });
            
            // Next spawn
            //srand((unsigned)time(0));
            m_next_leg_spawn = (LEG_DELAY_MS/2) + rand()%(1000);
        }
    }

    // // Spawning new turtles
    // m_next_turtle_spawn -= elapsed_ms * m_current_speed;
    // if (m_turtles.size() <= MAX_TURTLES && m_next_turtle_spawn < 0.f)
    // {
    // 	if (!spawn_turtle())
    // 		return false;
    
    // 	Turtle& new_turtle = m_turtles.back();
    
    // 	// Setting random initial position
    // 	new_turtle.set_position({ screen.x + 150, 50 + m_dist(m_rng) * (screen.y - 100) });
    
    // 	// Next spawn
    // 	m_next_turtle_spawn = (TURTLE_DELAY_MS / 2) + m_dist(m_rng) * (TURTLE_DELAY_MS/2);
    // }
    
    // // Spawning new fish
    // m_next_fish_spawn -= elapsed_ms * m_current_speed;
    // if (m_fish.size() <= MAX_FISH && m_next_fish_spawn < 0.f)
    // {
    // 	if (!spawn_fish())
    // 		return false;
    // 	Fish& new_fish = m_fish.back();
    
    // 	new_fish.set_position({ screen.x + 150, 50 + m_dist(m_rng) *  (screen.y - 100) });
    
    // 	m_next_fish_spawn = (FISH_DELAY_MS / 2) + m_dist(m_rng) * (FISH_DELAY_MS / 2);
    // }

    if ((int)difftime( time(0), start) == m_counter)
        timer_update();
    return true;
}

void World::timer_update()
{

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
    //const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
    glClearColor(1.0, 1.0, 1.0, 1.0);//0.05, 0.09, 0.16, 1.0);//clear_color[1], clear_color[1], clear_color[0], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)w;// *0.5;
    float bottom = (float)h;// *0.5;
    
    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
    
    m_worldtexture.draw(projection_2D);
    //TODO: Drawing entities
    for (auto& arms : m_arms)
        arms.draw(projection_2D);
    for (auto& legs : m_legs)
        legs.draw(projection_2D);
    // for (auto& turtle : m_turtles)
    // 	turtle.draw(projection_2D);
    // for (auto& fish : m_fish)
    // 	fish.draw(projection_2D);
    //m_salmon.draw(projection_2D);

    m_toolboxManager.draw(projection_2D);
    m_player1.draw(projection_2D);
    m_player2.draw(projection_2D);
    
    // TODO: will need to spawn random arms and legs
    //m_arms.draw(projection_2D);
    //m_legs.draw(projection_2D);
    m_water.draw(projection_2D);
    m_freeze.draw(projection_2D);
    
    // Presenting
    glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
    return glfwWindowShouldClose(m_window);
}

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

//bool World::spawn_turtle()
//{
//	Turtle turtle;
//	if (turtle.init())
//	{
//		m_turtles.emplace_back(turtle);
//		return true;
//	}
//	fprintf(stderr, "Failed to spawn turtle");
//	return false;
//}

// Creates a new fish and if successfull adds it to the list of fish
//bool World::spawn_fish()
//{
//	Fish fish;
//	if (fish.init())
//	{
//		m_fish.emplace_back(fish);
//		return true;
//	}
//	fprintf(stderr, "Failed to spawn fish");
//	return false;
//}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE SALMON MOVEMENT HERE
    // key is of 'type' GLFW_KEY_
    // action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    //	if (action == GLFW_PRESS && key == GLFW_KEY_UP)
    //		m_salmon.press_key(0);
    //	if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
    //		m_salmon.press_key(1);
    //	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
    //		m_salmon.press_key(2);
    //	if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
    //		m_salmon.press_key(3);
    //
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
    //	m_current_speed = fmax(0.f, m_current_speed);
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE SALMON ROTATION HERE
    // xpos and ypos are relative to the top-left of the window, the salmon's 
    // default facing direction is (1, 0)
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    //	vec2 curr_pos = m_salmon.get_position();
    //
    //	float delta_x = curr_pos.x - xpos;
    //	float delta_y = curr_pos.y - ypos;
    //
    //	float rad = atan( delta_y / delta_x );
    //
    //	if (delta_x > 0) rad += M_PI;
    //
    //	m_salmon.set_rotation(rad);
}
