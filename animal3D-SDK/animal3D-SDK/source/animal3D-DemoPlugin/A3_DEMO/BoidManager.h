#pragma once
#include <vector>

class Boid;
struct a3_NetworkingManager;
struct  a3_DemoState;
class Vector2;

class BoidManager
{
public:
	BoidManager();
	~BoidManager();

	void SpawnNewBoid(Vector2 pos, Vector2 velocity, bool active = false,  float rotation = 0, float radius = 5);

	void UpdateBoids(a3_NetworkingManager* net, a3_DemoState* demoState);

	void RenderBoids(a3_NetworkingManager* net, a3_DemoState* demoState);

	void ProcessOutbounds(a3_NetworkingManager* net);

	void UpdateSingleBoid(int boidIndex, float x, float y);

	void DetectCollisions();

	int boidID = 0;

	const static int BOIDS_PER_USER = 30;

private:
	std::vector<Boid*> boids;
};