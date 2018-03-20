
#pragma once

#include "common.hpp"
#include "toolboxManager.hpp"
#include "limbsManager.hpp"
#include "Limb.h"
#include "player1.hpp"
#include "player2.hpp"
#include "zombie.hpp"
#include "water.hpp"
#include "mud.hpp"
#include "Ice.hpp"
#include "tree.hpp"
#include "worldtexture.hpp"
#include "viewHelper.hpp"
#include "button.hpp"
#include "antidote.hpp"
#include "MapGrid.h"
#include "JPS.h"
#include "ToolManager.h"
#include "missile.hpp"
#include "bomb.hpp"
#include "armour.hpp"
#include <vector>
#include <Box2D.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <time.h>
#include <random>

#define SDL_MAIN_HANDLED

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
    void populateMapCollisionPoints();

    void use_tool_1(int tool_number);
    void use_tool_2(int tool_number);

    //start button
    //bool buttonclicked();
    void explode();
    void autoExplode();
    void use_bomb(float ms);
    //void init_use_bomb(float ms);
    bool useBomb;

private:
    bool spawn_freeze();
    bool spawn_water();
    bool spawn_missile();
    bool spawn_armour();
    bool spawn_bomb();

    void shift_1(bool droppedAntidote);
    void shift_2(bool droppedAntidote);

	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, int button, int action, int mod);

	void collect_freeze(Ice freeze, int player, float index);
	void collect_water(Water water, int player, float index);

    void collect_bomb(Bomb bomb, int player, float index);
    void collect_missile(Missile missile, int player, float index);
    void collect_armour(Armour armour, int player, float index);

	void add_to_broadphase(int w, int h, float posx, float posy, void*);

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
    time_t armourTime_p1;
    time_t armourTime_p2;

	// Game entities
	Worldtexture m_worldtexture;
	ToolboxManager m_toolboxManager;
	ToolManager toolManager;
	LimbsManager m_limbsManager;
	Player1 m_player1;
	Player2 m_player2;
	Zombie m_zombie;
	Antidote m_antidote;

    Tree m_tree;

    //Mud mud;
    
    std::vector<Ice> m_freeze;
    std::vector<Water> m_water;
    std::vector<Missile> m_missile;
    std::vector<Bomb> m_bomb;
    std::vector<Armour> m_armour;

    std::vector<Ice> m_freeze_collected_1;
    std::vector<Water> m_water_collected_1;
    std::vector<Ice> m_freeze_collected_2;
    std::vector<Water> m_water_collected_2;
    std::vector<Missile> m_missile_collected_1;
    std::vector<Missile> m_missile_collected_2;
    std::vector<Bomb> m_bomb_collected_1;
    std::vector<Bomb> m_bomb_collected_2;
    std::vector<Armour> m_armour_collected_1;
    std::vector<Armour> m_armour_collected_2;
    
    std::vector<Mud> m_mud_collected;

    std::vector<Bomb> used_bombs;

	MapGrid *mapGrid;

    std::vector<vec2> mapCollisionPoints;

	float m_next_arm_spawn;
	float m_next_leg_spawn;
	float m_next_spawn;

    int immobilize;
    bool armourInUse_p1;
    bool armourInUse_p2;
    bool droptool_p1;
    bool droptool_p2;
    bool explosion;

	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1c

	Button m_button;
};
