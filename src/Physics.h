//
// Created by Mark Pedersen on 2018-03-13.
//

#ifndef ZOMBIE_PHYSICS_H
#define ZOMBIE_PHYSICS_H

#include <Box2D.h>
#include <vector>

class Physics
{
public:
    Physics();
    virtual ~Physics();
    b2World* GetWorld();
    void DestroyWorld();
    void DestroyBodies(std::vector<b2Body*> bodies);
    void DestroyBody(b2Body* body);
    b2Body* CreateBody(b2BodyDef* bodyDef);
    void Update(double deltaTime);
    b2Contact* GetContactList();
    static Physics* Get() {
        static Physics instance;
        instance.Init();
        return &instance;
    }

private:
    Physics(Physics const&);
    void operator=(Physics const&);
    b2World* m_world;
    void Init();

};

#endif //ZOMBIE_PHYSICS_H
