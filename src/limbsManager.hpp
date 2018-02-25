#pragma once

#include "common.hpp"
#include "viewHelper.hpp"

class LimbsManager
{
    static Texture arms_texture;
public:
    // initialize a limbsManager
    bool init();
    
    // Renders the existing limbs
    void draw(const mat3& projection)override;

   
   //spawn new arm/leg in random
   //clusterLegs return a mapping? or void and just updates the mapping field
   //clusterArms return a mapping?
   //clusterLimbs return a mapping?
   

    
    void destroy();
    
    // vec2 get_bounding_box()const;
    
private:
//list of arms
//list of legs
//list of centroids for arms
//list of centroids for legs
//list of centroids for limbs
//mapping of arms to arm centroids
//mapping for legs to leg centroids
//mapping of limbs to limb centroids
    
};