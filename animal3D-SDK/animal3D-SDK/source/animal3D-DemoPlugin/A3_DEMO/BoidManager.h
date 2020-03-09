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

	void SpawnNewBoid(Vector2 pos, Vector2 velocity, bool active = false,  float rotation = 0, float radius = 5, float r = 1, float g = 0, float b = 0, int id = 0);

	void UpdateBoids(a3_NetworkingManager* net, a3_DemoState* demoState);

	void RenderBoids(a3_NetworkingManager* net, a3_DemoState* demoState);

	void ProcessOutbounds(a3_NetworkingManager* net, a3_DemoState* demostate);

	void UpdateSingleBoid(int boidIndex, float x, float y);

	void DetectCollisions(bool everyone = false);
	void SetBoidActive(int boidIndex, bool active = true);

	Boid* getBoid(int index) { return boids[index]; }

	int boidID = 0;

	const static int BOIDS_PER_USER = 30;

	const float TIMESTEP = 2.0f;

private:
	std::vector<Boid*> boids;

	float currentTime = 0.0f;
};