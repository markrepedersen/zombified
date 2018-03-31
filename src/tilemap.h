////
//// Created by Mark Pedersen on 2018-03-20.
////
//
//#ifndef ZOMBIE_TILEMAP_H
//#define ZOMBIE_TILEMAP_H
//
//#include "common.hpp"
//#include <map>
//#include <regex>
//#include <vector>
//#include "terrain.h"
//
//
//using std::map;
//using std::regex;
//using std::vector;
//using std::string;
//
//class tilemap : public Renderable
//{
//public:
//    void init();
//    bool check_collision(vec2);
//    void parseMap();
//    vector<string> tileSplit(string,string);
//    void draw(const mat3& projection)override;
//    void destroy();
//    void addCollision(vec2);
//
//private:
//    map<vec2, int> coordMap;
//
//    //Terrain terrain;
//    vector<Terrain> terrain_pieces;
//    vector<int> unpassable = { 50,51,52,70,71,72,90,91,92,10,11,30,31 };
//    int multiple = 28;
//
//};
//
//
//
//
//#endif //ZOMBIE_TILEMAP_H
