//
// Created by Mark Pedersen on 2018-03-12.
//

#include "PhysicalBody.h"

void PhysicalBody::addBodyToWorld(b2World *world) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x / pixelsPerMeter, position.y / pixelsPerMeter);
    bodyDef.userData = this;
    bodyDef.fixedRotation = true;
    this->body = world->CreateBody(&bodyDef);
}

void PhysicalBody::addCircularFixtureToBody(float radius)
{
    b2CircleShape shape;
    shape.m_radius = radius;
    this->createFixture(&shape);
}

void PhysicalBody::addRectangularFixtureToBody(float width, float height)
{
    b2PolygonShape shape;
    shape.SetAsBox(width, height);
    this->createFixture(&shape);
}

void PhysicalBody::createFixture(b2Shape* shape)
{
    // note that friction, etc. can be modified later by looping
    // over the body's fixtures and calling fixture->SetFriction()
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.7f;
    fixtureDef.restitution = 0.1f;
    fixtureDef.filter.categoryBits = kFilterCategorySolidObject;
    fixtureDef.filter.maskBits = 0xffff;
    this->body->CreateFixture(&fixtureDef);
}

b2Body *PhysicalBody::getBody() {
    return body;
}