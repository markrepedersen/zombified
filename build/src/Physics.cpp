////
//// Created by Mark Pedersen on 2018-03-13.
////
//
//#include "Physics.h"
//
//Physics::Physics() {}
//
//b2World* Physics::GetWorld()
//{
//    return m_world;
//}
//
//Physics::~Physics()
//{
//    DestroyWorld();
//}
//
//void Physics::DestroyWorld()
//{
//    if(m_world != NULL)
//    {
//        delete m_world;
//        m_world = NULL;
//    }
//}
//
//void Physics::DestroyBodies(std::vector<b2Body*> bodies)
//{
//    for(b2Body* body : bodies)
//    {
//        DestroyBody(body);
//    }
//}
//
//void Physics::DestroyBody(b2Body* body)
//{
//    m_world->DestroyBody(body);
//}
//
//void Physics::Init()
//{
//    if(m_world == NULL)
//    {
//        b2Vec2 gravity(0.0f, 9.8f);
//        m_world = new b2World(gravity);
//        m_world->SetAllowSleeping(true);
//        m_world->SetContinuousPhysics(true);
//    }
//}
//
//void Physics::Update(double deltaTime)
//{
//    if(m_world)
//    {
//        m_world->Step(deltaTime / 60, 8, 3);
//        m_world->ClearForces();
//    }
//}
//
//b2Body* Physics::CreateBody(b2BodyDef* bodyDef)
//{
//    return m_world->CreateBody(bodyDef);
//}
//
//b2Contact* Physics::GetContactList()
//{
//    return m_world->GetContactList();
//}
