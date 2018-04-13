#pragma once

#include "common.hpp"

class Kinetic : public Renderable {
public:
	// polymorphic collision handler
	// internal functions must be implemented in all game objects that may collide with other objects
	void on_collide(Kinetic *collider);
	virtual void move(vec2);
	virtual vec2 getAABB() = 0;
private:
	virtual void on_player1_collision(Kinetic *player) = 0;
	virtual void on_player2_collision(Kinetic *player) = 0;
	virtual void on_antidote_collision(Kinetic *antidote) = 0;
	virtual void on_limb_collision(Kinetic *limb) = 0;
	virtual void on_armour_collision(Kinetic *player) = 0;
	virtual void on_explosion_collision(Kinetic *explosion) = 0;
	virtual void on_ice_collision(Kinetic *ice) = 0;
	virtual void on_missile_collision(Kinetic *missile) = 0;
	virtual void on_water_collision(Kinetic *water) = 0;
	virtual void on_zombie_collision(Kinetic *zombie) = 0;
};