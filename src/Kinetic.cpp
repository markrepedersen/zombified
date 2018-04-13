#include "Kinetic.h"
#include "player1.hpp"
#include "explosion.hpp"
#include "zombie.hpp"
#include "MapGrid.h"

void Kinetic::move(vec2 pos) {
    MapGrid::GetInstance()->removeOccupant(this);
    this->m_position += pos;
    MapGrid::GetInstance()->addOccupant(this);
}

void Kinetic::collide(Kinetic *collider) {
    if (auto player1 = dynamic_cast<Player1 *>(collider)) {
        on_player1_collision(player1);
    } else if (auto player2 = dynamic_cast<Player2 *>(collider)) {
        on_player2_collision(player2);
    } else if (auto antidote = dynamic_cast<Antidote *>(collider)) {
        on_antidote_collision(antidote);
    } else if (auto limb = dynamic_cast<Limb *>(collider)) {
        on_limb_collision(limb);
    } else if (auto armour = dynamic_cast<Armour *>(collider)) {
        on_armour_collision(armour);
    } else if (auto explosion = dynamic_cast<Explosion *>(collider)) {
        on_explosion_collision(explosion);
    } else if (auto ice = dynamic_cast<Ice *>(collider)) {
        on_ice_collision(ice);
    } else if (auto missile = dynamic_cast<Missile *>(collider)) {
        on_missile_collision(missile);
    } else if (auto water = dynamic_cast<Water *>(collider)) {
        on_water_collision(water);
    } else if (auto zombie = dynamic_cast<Zombie *>(collider)) {
        on_zombie_collision(zombie);
    }
}

