////
//// Created by Mark Pedersen on 2018-03-12.
////
//
//#ifndef ZOMBIE_PHYSICALBODY_H
//#define ZOMBIE_PHYSICALBODY_H
//
//
//#include <Box2D.h>
//#include "common.hpp"
//
//enum
//{
//    kFilterCategoryLevel = 0x01,
//    kFilterCategorySolidObject = 0x02,
//    kFilterCategoryNonSolidObject = 0x04
//};
//
//class PhysicalBody {
//private:
//    void createFixtures();
//    b2Fixture *createFixture(b2Shape* shape);
//    b2Fixture *addRectangularFixtureToBody(float width, float height);
//    b2Fixture *addAngledRectangularFixtureToBody(float width, float height, b2Vec2 centre, float angle);
//    void addCircularFixtureToBody(float radius);
//protected:
//    void addBodyToWorld();
//    b2Body *getBody() {
//        return body;
//    };
//
//    float bodyWidth;
//    float bodyHeight;
//    vec2 m_position;
//    b2Body* body;
//    b2Fixture* bodyFixture;
//    b2Fixture *headFixture;
//    b2Fixture* legFixture;
//    b2Fixture* leftFixture;
//    b2Fixture* rightFixture;
//    const float pixelsPerMeter =  32.0f;
//};
//
//
//#endif //ZOMBIE_PHYSICALBODY_H
