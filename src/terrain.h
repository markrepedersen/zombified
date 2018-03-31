////
//// Created by Mark Pedersen on 2018-03-20.
////
//
//#ifndef ZOMBIE_TERRAIN_H
//#define ZOMBIE_TERRAIN_H
//
//#include "common.hpp"
//#include <vector>
//
//using std::vector;
//
//class Terrain : public Renderable
//{
//    static Texture terrain_texture;
//
//public:
//    bool init(int,int,int,Effect);
//
//    void clear();
//
//    void destroy();
//
//    void draw(const mat3& projection)override;
//
//    vector<vec2> get_nodes()const;
//
//    vector<vec2> get_mini_nodes()const;
//
//    bool return_state();
//
//    void getEffect(Effect);
//
//private:
//    vec2 m_position; // Window coordinates
//    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
//
//    vector<vec2> nodes;
//
//    vector<vec2> mini_nodes;
//    bool is_active = false;
//
//    Effect eff;
//};
//
//
//
//#endif //ZOMBIE_TERRAIN_H
