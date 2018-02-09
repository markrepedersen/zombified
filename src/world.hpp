
#pragma once

// internal
#include "common.hpp"
//#include "salmon.hpp"
//#include "turtle.hpp"
//#include "fish.hpp"
#include "toolboxManager.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include "arms.hpp"
#include "legs.hpp"
#include "water.hpp"
#include "freeze.hpp"
#include "worldtexture.hpp"
#include "viewHelper.hpp"
// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

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

private:
	// Generates a new turtle
	bool spawn_arms();
	bool spawn_legs();

	// Generates a new fish
	//bool spawn_fish();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// Window hjandle
	GLFWwindow* m_window;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int m_points;

	// Game entities
    Worldtexture m_worldtexture;
    ToolboxManager m_toolboxManager;
    Player1 m_player1;
    Player2 m_player2;
    //Arms m_arms;
    //Legs m_legs;
    Water m_water;
    Freeze m_freeze;
//	Salmon m_salmon;
//	std::vector<Turtle> m_turtles;
//	std::vector<Fish> m_fish;
    std::vector<Arms> m_arms;
    std::vector<Legs> m_legs;
//
//	float m_current_speed;
	float m_next_arm_spawn;
    float m_next_leg_spawn;
//	float m_next_fish_spawn;
	
	//Mix_Music* m_background_music;
	//Mix_Chunk* m_salmon_dead_sound;
	//Mix_Chunk* m_salmon_eat_sound;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1c
};
