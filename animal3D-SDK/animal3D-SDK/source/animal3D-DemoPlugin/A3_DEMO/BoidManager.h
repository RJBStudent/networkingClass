#pragma once
#include <vector>

class Boid;
struct a3_NetworkingManager;
struct  a3_DemoState;

class BoidManager
{
public:
	BoidManager();
	~BoidManager();

	void SpawnNewBoid();

	void UpdateBoids(a3_NetworkingManager* net, a3_DemoState* demoState);

	void RenderBoids(a3_NetworkingManager* net, a3_DemoState* demoState);

	void ProcessOutbounds(a3_NetworkingManager* net);

	void UpdateSingleBoid(int boidIndex, float x, float y);

	int boidID = 0;

	const static int BOIDS_PER_USER = 10;

private:
	std::vector<Boid> boids;
};