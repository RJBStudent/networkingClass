#pragma once

#include "Vector2.h"

class Boid
{
public:
	Boid(Vector2 newPos = Vector2(0, 0), Vector2 newVel = Vector2(0, 0), float newRot = 0);
	~Boid();

	void Update(float dt);

	Vector2 position = Vector2(0, 0);
	Vector2 velocity = Vector2(0, 0);
	float rotation = 0;
};