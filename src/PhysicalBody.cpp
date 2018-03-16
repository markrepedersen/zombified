//
// Created by Mark Pedersen on 2018-03-12.
//

#include "PhysicalBody.h"
#include "Physics.h"

void PhysicalBody::addBodyToWorld() {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(m_position.x / pixelsPerMeter, m_position.y / pixelsPerMeter);
    bodyDef.fixedRotation = true;
    this->body = Physics::Get()->CreateBody(&bodyDef);
    this->body->SetBullet(true);
    createFixtures();
}

void PhysicalBody::addCircularFixtureToBody(float radius)
{
    b2CircleShape shape;
    shape.m_radius = radius;
    this->createFixture(&shape);
}

b2Fixture *PhysicalBody::addRectangularFixtureToBody(float width, float height)
{
    b2PolygonShape shape;
    shape.SetAsBox(width, height);
    return this->createFixture(&shape);
}

b2Fixture *PhysicalBody::addAngledRectangularFixtureToBody(float width, float height, b2Vec2 centre, float angle)
{
    b2PolygonShape shape;
    shape.SetAsBox(width, height, centre, angle);
    return this->createFixture(&shape);
}

b2Fixture *PhysicalBody::createFixture(b2Shape* shape)
{
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.7f;
    fixtureDef.restitution = 0.1f;
    fixtureDef.filter.categoryBits = kFilterCategorySolidObject;
    fixtureDef.filter.maskBits = 0xffff;
    return this->body->CreateFixture(&fixtureDef);
}

void PhysicalBody::createFixtures() {
    b2PolygonShape bodyBox;
    bodyBox.SetAsBox(bodyWidth/2, bodyHeight/2);

    bodyFixture = addRectangularFixtureToBody(bodyWidth/2, bodyHeight/2);
    legFixture = addRectangularFixtureToBody(bodyWidth/2-2, bodyHeight/2 - 10);
    rightFixture = addAngledRectangularFixtureToBody(bodyWidth/6, bodyHeight/2 - 1, b2Vec2(bodyWidth/2, 0), 0);
    leftFixture = addAngledRectangularFixtureToBody(bodyWidth/6, bodyHeight/2 - 1, b2Vec2(-bodyWidth/2, 0), 0);
}