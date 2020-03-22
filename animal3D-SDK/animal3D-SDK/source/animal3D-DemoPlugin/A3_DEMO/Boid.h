#pragma once

#include "Vector2.h"
struct a3_DemoState;

class Boid
{
public:
	Boid(bool activeState,Vector2 newPos = Vector2(0, 0), Vector2 newVel = Vector2(0, 0), float newRot = 0, float newRadius = 5, float newR = 1, float newG = 0, float newB = 0, int newID = 0);
	~Boid();

	void Update(a3_DemoState* demoState, float dt);
	void Render(a3_DemoState* demoState);

	Vector2 position = Vector2(0, 0);
	Vector2 velocity = Vector2(0, 0);
	float rotation = 0;
	bool active = false;
	float radius = 0;
	float r, g, b;
	int myID;

	Vector2 currentPos = Vector2(0, 0);
	Vector2 targetPos = Vector2(0, 0);
	Vector2 currentVel = Vector2(0, 0);
	Vector2 targetVel = Vector2(0, 0);
	float lerpValue;
};