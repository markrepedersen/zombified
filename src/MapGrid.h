//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_MAPGRID_H
#define ZOMBIE_MAPGRID_H

#include "common.hpp"
#include <vector>
#include "JPS.h"
#include "Limb.h"
#include "antidote.hpp"
#include "Ice.hpp"
#include "missile.hpp"
#include "player1.hpp"

#define SCALING 500

enum Terrain {
    REGULAR = 1,
    POOP,
    WATER,
    ICE
};

typedef struct Pos point;

struct Pos {
    int x, y;
};

struct AABBC {
public:
    AABBC(int x, int y, int radius) : x(x), y(y), radius(radius) {
        cornerPoints.push_back((point) {x - radius, y - radius});
        cornerPoints.push_back((point) {x + radius, y - radius});
        cornerPoints.push_back((point) {x - radius, y + radius});
        cornerPoints.push_back((point) {x + radius, y + radius});
    }
    int x;
    int y;
    int radius;
    std::vector<point> getCornerPoints() {
        return cornerPoints;
    }
private:
    std::vector<point> cornerPoints;
};
struct AABB {
public:
    AABB(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {
        cornerPoints.push_back((point) {x - width, y - width});
        cornerPoints.push_back((point) {x + width, y - width});
        cornerPoints.push_back((point) {x - width, y + width});
        cornerPoints.push_back((point) {x + width, y + width});
    }
    int x;
    int y;
    int width;
    int height;
    std::vector<point> getCornerPoints() {
        return cornerPoints;
    }
private:
    std::vector<point> cornerPoints;
};

struct Node {
public:
    Node(int sexiness, bool walkable, int terrain) : sexiness(sexiness), walkable(walkable), terrain(terrain) {};
    const std::vector<Renderable*> &getPopulation() {
        return population;
    }
    void addCollider(Renderable* collider) {
        if(std::find(population.begin(), population.end(), collider) == population.end()) {
            population.push_back(collider);
        }
    }

    void removeCollider(Renderable* collider) {
        for (auto it = population.begin(); it != population.end();) {
            if (*it == collider) {
                population.erase(it);
                break;
            }
        }
    }
    int sexiness;
    bool walkable;
    int terrain;
private:
    std::vector<Renderable*> population;
};

class MapGrid {
public:
    MapGrid()=default;
    ~MapGrid()=default;
    static MapGrid* Get(int x, int y) {
        if (instance == 0) {
            instance = new MapGrid();
            instance->Init(x, y);
        }
        return instance;
    }
    static MapGrid* GetInstance() {
        return instance;
    }
    void Init(int x, int y);
    bool operator()(unsigned x, unsigned y) const;
    bool isOccupied(int x, int y)const;
    void addOccupant(int x, int y, Renderable* occupant);
    void addOccupant(int radius, Renderable* occupant);
    void removeOccupant(int x, int y, Renderable *occupant);
    void removeOccupant(int radius, Renderable *occupant);
private:
    MapGrid(MapGrid const&);
    std::vector<int> possibleCollisions(int x, int y);
    void operator=(MapGrid const&);
    unsigned width, height;
    std::vector<std::vector<Node*>> mapdata;
    static MapGrid *instance;
};

#endif //ZOMBIE_MAPGRID_H
