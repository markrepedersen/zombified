//
// Created by Mark Pedersen on 2018-02-24.
//

#ifndef ZOMBIE_LIMB_H
#define ZOMBIE_LIMB_H

#include "JPS.h"
#include "viewHelper.hpp"
#include "common.hpp"
#include <time.h>

class Limb : public Renderable {

static Texture leg_texture;
static Texture arm_texture;

public:
    bool init(std::string type);

    void draw(const mat3& projection)override;

    void destroy();

    vec2 get_bounding_box()const;
    
    void setLastPath(JPS::PathVector path);

    void setCurrentPath(JPS::PathVector path);

    void setCurrentTarget(vec2 target);

    void setLastTarget(vec2 target);

    vec2 getCurrentTarget()const;

    vec2 getLastTarget()const;

    JPS::PathVector getCurrentPath()const;

    JPS::PathVector getLastPath()const;

    vec2 get_position()const;

    void set_position(vec2 position);

    void move(vec2 pos, float ms);

    std::string getLimbType();

    bool isInitialized() {return initialized;}

    void setInitialized(bool flag) {this->initialized = flag;}

    void animate(float ms);
    
    void initLegTime();
    time_t getLegTime();
    
    time_t legTime;

protected:
    vec2 cur_target;
    vec2 last_target;
    JPS::PathVector lastPath;
    JPS::PathVector currentPath;
    vec2 m_position;
    vec2 m_scale;
    std::string type;
    bool initialized = false;

    // current frame in animation
    int curr_frame_limb = 0;
    // frame to draw and previous frame in sprite
    int sprite_frame_index_limb = 0;
    // sprite information
    int sprite_width_arm = 522;
    int sprite_height_arm = 242;
    int sprite_width_leg = 410;
    int sprite_height_leg = 240;
    int num_rows_limb = 1;
    int num_cols_limb = 5;
    int frames_arm [5] = {0, 1, 2, 3, 4};
    // animation timing
    int frame_time_limb = 150;

    float tot_elapsed_time = 0;
};

#endif //ZOMBIE_LIMB_H
