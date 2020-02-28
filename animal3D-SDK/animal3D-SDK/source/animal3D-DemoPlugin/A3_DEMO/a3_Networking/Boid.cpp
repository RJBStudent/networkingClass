#include "../Boid.h"
#include "A3_DEMO/a3_DemoState.h"
#include "animal3D/animal3D.h"

Boid::Boid(bool activeState, Vector2 newPos, Vector2 newVel, float newRot, float newRadius)
{
	active = activeState;
	position = newPos;
	velocity = newVel;
	rotation = newRot;
	radius = newRadius;
}

Boid::~Boid()
{

}

void Boid::Update(a3_DemoState* demoState, float dt)
{
	if (!active)
		return;
	position += velocity * dt;

	a3f32 height = (a3f32)demoState->windowHeight;
	a3f32 width = (a3f32)demoState->windowWidth;

	if (position.x > width)
	{
		position.x = -width;
	}
	if (position.x < -width)
	{
		position.x = width;
	}
	if (position.y > height)
	{
		position.y = -height;
	}
	if (position.y < -height)
	{
		position.y = height;
	}

}

void Boid::Render(a3_DemoState* demoState)
{
	if (!active)
		return;
	a3f32 x = (a3f32)position.x;
	a3f32 y = (a3f32)position.y;
	a3f32 height = (a3f32)demoState->windowHeight;
	a3f32 width = (a3f32)demoState->windowWidth;
	a3textDraw(demoState->text, (x / width), (y / height), -1, 1, 0, 0, 1, "A");
}

