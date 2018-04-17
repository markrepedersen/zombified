#pragma once

#include "common.hpp"
#include "JPS.h"
#include "viewHelper.hpp"

class Zombie : public Renderable
{
    static Texture zombie_texture;
public:
    // Creates all the associated render resources and default transform
    bool init();
    
    // Renders the salmon
    void draw(const mat3& projection)override;

    void set_position(vec2 position);
    // Returns the current salmon position
    vec2 get_position()const;
    
    void set_scale(vec2 scale);
    
    void destroy();
    
    vec2 get_bounding_box()const;

    void setLastPath(JPS::PathVector path);

    void setCurrentPath(JPS::PathVector path);

    void setCurrentTarget(vec2 target);

    void setLastTarget(vec2 target);

    void setAttackTimeout(float timeout);

    void setMass(float mass);

    vec2 getCurrentTarget()const;

    vec2 getLastTarget()const;

    JPS::PathVector getCurrentPath()const;

    JPS::PathVector getLastPath()const;
    
    float getAttackTimeout();

    float getMass();

    void move(vec2 pos, float ms);

    void animate(float ms, int direction);

    bool isInitialized() {return initialized;}

    void setInitialized(bool flag) {this->initialized = flag;}

protected:
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
    JPS::PathVector currentPath;
    vec2 m_scale;
    bool initialized = false;
    float attack_timeout;
    float mass;

        // current frame in animation
    int curr_frame_zombie = 0;
    // frame to draw and previous frame in sprite
    int sprite_frame_index_zombie = 0;
    // sprite information
    int sprite_width_zombie = 195;
    int sprite_height_zombie = 305;
    int num_rows_zombie = 1;
    int num_cols_zombie = 10;
    int frames_zombie_up [2] = {8, 9};
    int frames_zombie_left [3] = {2,3,4};
    int frames_zombie_right[3] = {5,6,7};
    int frames_zombie_down [2] = {0, 1};

    // animation timing
    int frame_time_zombie = 50;

    float tot_elapsed_time = 0;
};