#pragma once
#include <vector>

class Boid;

class BoidManager
{
public:
	BoidManager();
	~BoidManager();

	SpawnNewBoid();

private:
	std::vector<Boid> boids;
};