#pragma once

#include "Vector2.h"
struct a3_DemoState;

class Boid
{
public:
	Boid(bool activeState,Vector2 newPos = Vector2(0, 0), Vector2 newVel = Vector2(0, 0), float newRot = 0, int newRadius = 5);
	~Boid();

	void Update(a3_DemoState* demoState, float dt);
	void Render(a3_DemoState* demoState);

	Vector2 position = Vector2(0, 0);
	Vector2 velocity = Vector2(0, 0);
	float rotation = 0;
	bool active = false;
	int radius = 0;
};