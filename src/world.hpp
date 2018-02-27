
#pragma once

#include "common.hpp"
#include "toolboxManager.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include "arms.hpp"
#include "legs.hpp"
#include "water.hpp"
#include "freeze.hpp"
#include "worldtexture.hpp"
#include "viewHelper.hpp"
#include "button.hpp"
#include "antidote.hpp"
#include "MapGrid.h"
#include "JPS.h"
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <time.h>
#include <random>
#include <unordered_set>

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;
    
    void timer_update();
    void check_add_tools(vec2 screen);
    bool random_spawn(float elapsed_ms, vec2 screen);
    
    void use_tool_1(int tool_number);
    void use_tool_2(int tool_number);
    
    //start button
    //bool buttonclicked();

private:
	// Generates a new turtle
	bool spawn_arms();
	bool spawn_legs();
    bool spawn_freeze();
    bool spawn_water();
    
    void shift_1();
    void shift_2();

	// Generates a new fish
	//bool spawn_fish();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	//void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
    void on_mouse_move(GLFWwindow* window, int button, int action, int mod);
    
    void collect_freeze(Freeze freeze, int player, float index);
    void collect_water(Water water, int player, float index);

	void computePaths(float ms);
    
private:
	// Window handle
	GLFWwindow* m_window;
    
    // true if the start button was pressed to start the main game world
    bool game_started;
    bool game_over;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int m_min;
    unsigned int m_sec;
    unsigned int timeDelay;
    time_t start;
    time_t freezeTime;

	// Game entities
    Worldtexture m_worldtexture;
    ToolboxManager m_toolboxManager;
    Player1 m_player1;
    Player2 m_player2;
    Antidote m_antidote;

    std::vector<Arms> m_arms;
    std::vector<Legs> m_legs;
    std::vector<Freeze> m_freeze;
    std::vector<Water> m_water;

    std::vector<Freeze> m_freeze_collected_1;
    std::vector<Water> m_water_collected_1;
    std::vector<Freeze> m_freeze_collected_2;
    std::vector<Water> m_water_collected_2;

	MapGrid *mapGrid;

	float m_next_arm_spawn;
    float m_next_leg_spawn;
    float m_next_spawn;

    int check_freeze_used;
	
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1c
    
    Button m_button;
};
