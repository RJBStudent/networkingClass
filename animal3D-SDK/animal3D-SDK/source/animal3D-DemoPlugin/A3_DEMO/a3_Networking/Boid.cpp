#include "../Boid.h"
#include "A3_DEMO/a3_DemoState.h"
#include "animal3D/animal3D.h"
#include <stdio.h>
#include <cstdlib>

Boid::Boid(bool activeState, Vector2 newPos, Vector2 newVel, float newRot, float newRadius, float newR, float newG, float newB, int newID)
{
	active = activeState;
	position = newPos;
	velocity = newVel;
	rotation = newRot;
	radius = newRadius;
	r = newR;
	b = newB;
	g = newG;
	myID = newID;
	lerpValue = 2;
	currentPos = velocity;
	targetPos = velocity;

	currentVel = newVel;
	targetVel = newVel;
}

Boid::~Boid()
{

}

void Boid::Update(a3_DemoState* demoState, float dt)
{
	if (!active)
		return;

	if (lerpValue > 1.05) // 1.05 for leeway
	{
		position += velocity * dt;

	}
	else
	{
		if (lerpValue == 0)
		{
			currentPos = position;
			currentVel = velocity;
		}
		position = Vector2::lerp(currentPos, targetPos, lerpValue);
		velocity = Vector2::lerp(currentVel, targetVel, lerpValue);
		lerpValue += .2f;

	}

	a3f32 height = (a3f32)demoState->windowHeight;
	a3f32 width = (a3f32)demoState->windowWidth;

	if (position.x > width)
	{
		//position.x = -width;
		
		velocity.x = -abs(velocity.x);
	}
	if (position.x < -width)
	{
		//position.x = width;
		velocity.x = abs(velocity.x);

	}
	if (position.y > height)
	{
		//position.y = -height;
		velocity.y = -abs(velocity.y);

	}
	if (position.y < -height)
	{
		//position.y = height;
		velocity.y = abs(velocity.y);

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
	a3textDraw(demoState->text, (x / width), (y / height), -1, r, g, b, 1, "%i",myID);
}

