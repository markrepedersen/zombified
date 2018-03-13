//
// Created by Mark Pedersen on 2018-03-12.
//

#ifndef ZOMBIE_PHYSICALBODY_H
#define ZOMBIE_PHYSICALBODY_H


#include <Box2D.h>
#include "common.hpp"

enum
{
    kFilterCategoryLevel = 0x01,
    kFilterCategorySolidObject = 0x02,
    kFilterCategoryNonSolidObject = 0x04
};

class PhysicalBody {
public:
    b2Body *getBody();
protected:
    b2Body* body;
    void addBodyToWorld(b2World *world);
    void createFixture(b2Shape* shape);
    void addRectangularFixtureToBody(float width, float height);
    void addCircularFixtureToBody(float radius);
    vec2 position;
    static const int pixelsPerMeter = 100;
};


#endif //ZOMBIE_PHYSICALBODY_H
