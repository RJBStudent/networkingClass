#include "../Boid.h"

Boid::Boid(Vector2 newPos, Vector2 newVel, float newRot)
{
	position = newPos;
	velocity = newVel;
	rotation = newRot;
}

Boid::~Boid()
{

}

Boid::Update(float dt)
{
	position += velocity * dt;
}