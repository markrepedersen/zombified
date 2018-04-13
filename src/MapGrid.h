//
// Created by Mark Pedersen on 2018-02-24.
//

#pragma once

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
    inline AABBC(int x, int y, int radius) : x(x), y(y), radius(radius) {
        cornerPoints.push_back((point) {x - radius, y - radius});
        cornerPoints.push_back((point) {x + radius, y - radius});
        cornerPoints.push_back((point) {x - radius, y + radius});
        cornerPoints.push_back((point) {x + radius, y + radius});
    }
    int x;
    int y;
    int radius;
    inline std::vector<point> getCornerPoints() {
        return cornerPoints;
    }
private:
    std::vector<point> cornerPoints;
};
struct AABB {
public:
    inline AABB(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {
        cornerPoints.push_back((point) {x - width < 0 ? 0 : x - width, y - width < 0 ? 0 : y - width});
        cornerPoints.push_back((point) {x + width, y - width < 0 ? 0 : y - width});
        cornerPoints.push_back((point) {x - width < 0 ? 0 : x - width, y + width});
        cornerPoints.push_back((point) {x + width, y + width});
    }
    int x;
    int y;
    int width;
    int height;
    inline std::vector<point> getCornerPoints() {
        return cornerPoints;
    }
private:
    std::vector<point> cornerPoints;
};

struct Node {
public:
    inline Node(int sexiness, bool walkable, int terrain) : sexiness(sexiness), walkable(walkable), terrain(terrain) {};
    inline const std::vector<Kinetic*> &getPopulation() {
        return population;
    }
    inline void addCollider(Kinetic* collider) {
        if(std::find(population.begin(), population.end(), collider) == population.end()) {
            population.push_back(collider);
        }
    }

    inline void removeCollider(Kinetic* collider) {
        for (auto it = population.begin(); it != population.end();++it) {
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
    std::vector<Kinetic*> population;
};

class MapGrid {
public:
    MapGrid()=default;
    ~MapGrid()=default;
    static MapGrid* Use(int x, int y) {
        if (instance == nullptr) {
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
    void addOccupant(Kinetic* occupant);
    void removeOccupant(Kinetic *occupant);
    std::vector<Kinetic*> getPossibleColliders(int x, int y);
    void processColliders();
private:
    MapGrid(MapGrid const&);
    std::vector<int> possibleCollisions(int x, int y);
    void operator=(MapGrid const&);
    unsigned width, height;
    std::vector<std::vector<Node*>> mapdata;
    static MapGrid *instance;
};