#include "../BoidManager.h"
#include "../Boid.h"
#include "../a3_NetworkingManager.h"
#include "../a3_DemoState.h"
#include "../NetworkMessages.h"
#include "RakNet/RakPeerInterface.h"

BoidManager::BoidManager()
{

}

BoidManager::~BoidManager()
{
	while (!boids.empty())
	{
		delete boids.back();
		boids.pop_back();
	}
}

void BoidManager::SpawnNewBoid(Vector2 pos, Vector2 velocity, bool active,  float rotation, float radius, float r, float g, float b, int id)
{
	Boid* newBoid = new Boid(active, pos, velocity, rotation, radius, r, g, b, id);
	boids.push_back(newBoid);
	printf("%f %f\n", newBoid->velocity.x, newBoid->velocity.y);
}

void BoidManager::UpdateBoids(a3_NetworkingManager* net, a3_DemoState* demoState)
{
	switch (net->dataPackageType)
	{
	case 1:
	{
		if (net->isServer)
		{
			for (unsigned int i = boidID * BOIDS_PER_USER; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++)
			{
				if (i < 0 || i > boids.size())
					continue;
				boids[i]->Update(demoState,(float)demoState->renderTimer->secondsPerTick);
			}
			DetectCollisions();
		}

	}
	break;
	case 2:
	{
		if (!net->isServer)
		{
			for (unsigned int i = boidID * BOIDS_PER_USER; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++)
			{
				if (i < 0 || i > boids.size())
					continue;
				boids[i]->Update(demoState, (float)demoState->renderTimer->secondsPerTick);
			}
			DetectCollisions();
		}
	}
	break;
	case 3:
	{
		for (unsigned int i = boidID * BOIDS_PER_USER; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++)
		{
			if (i < 0 || i > boids.size())
				continue;
			boids[i]->Update(demoState, (float)demoState->renderTimer->secondsPerTick);
		}
		DetectCollisions(true);
	}
	break;
	default:
		break;
	}
}

void BoidManager::RenderBoids(a3_NetworkingManager* net, a3_DemoState* demoState)
{
	switch (net->dataPackageType)
	{
	case 1:
	case 2:
	{
		if /*(!net->isServer)*/(true)
		{
		for (unsigned int i = 0; i < boids.size(); i++)
		{
			if (i < 0 || i > boids.size())
				continue;
			
			boids[i]->Render(demoState);
		}
		}
	}
	break;
	case 3:
	{
		for (unsigned int i = 0; i < boids.size(); i++)
		{
			if (i < 0 || i > boids.size())
				continue;
			boids[i]->Render(demoState);
		}
	}
	break;
	default:
		break;
	}
}

void BoidManager::UpdateSingleBoid(int boidIndex, float x, float y)
{
	if (boidIndex >= 0 || (unsigned)boidIndex < boids.size())
	{
		boids[boidIndex]->position.x = x;
		boids[boidIndex]->position.y = y;
		boids[boidIndex]->active = true;
	}
}


void BoidManager::ProcessOutbounds(a3_NetworkingManager* net, a3_DemoState* demostate)
{
	currentTime += (float)demostate->timer->secondsPerTick;
	if (currentTime < TIMESTEP)
	{
		return;
	}
	currentTime = 0.0f;
	switch (net->dataPackageType)
	{
	case 1:
	{
		if (net->isServer)
		{
			Vector2Message newMessage;
			newMessage.messageId = ID_SET_BOID_POS;
			for (unsigned int i = boidID * BOIDS_PER_USER; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++)
			{
				if (i < 0 || i > boids.size())
					continue;
				
				newMessage.idIndex[i - boidID * BOIDS_PER_USER] = i;
				newMessage.xValue[i - boidID * BOIDS_PER_USER] = boids[i]->position.x;
				newMessage.yValue[i - boidID * BOIDS_PER_USER] = boids[i]->position.y;
			}
			RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
			peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
		}
	}
	break;
	case 2:
	{
		if (!net->isServer)
		{
				Vector2Message newMessage;
				newMessage.messageId = ID_SET_BOID_POS;
				memset(newMessage.idIndex, -1, 30);
				
			for (unsigned int i = boidID * BOIDS_PER_USER, j = 0; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++, j++)
			{
				if (i < 0 || i > boids.size())
					continue;
				newMessage.idIndex[j] = i;
				newMessage.xValue[j] = boids[i]->position.x;
				newMessage.yValue[j] = boids[i]->position.y;
			}
				RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
				RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
				peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);
		}
	}
	break;
	case 3:
	{
		Vector2Message newMessage;
		newMessage.messageId = ID_SET_BOID_POS_VEL;
		
			for (unsigned int i = boidID * BOIDS_PER_USER, j =0; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++, j++)
			{
				if (i < 0 || i > boids.size())
					continue;
				newMessage.idIndex[j] = i;
				//POSITIONS
				newMessage.xValue[j*2] = boids[i]->position.x;
				newMessage.yValue[j*2] = boids[i]->position.y;
				//VELOCITIES
				newMessage.xValue[j*2+1] = boids[i]->velocity.x;
				newMessage.yValue[j*2+1] = boids[i]->velocity.y;
			}
			RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
			if (net->isServer)
			{
				peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
			}
			else
			{
				RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
				peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);
			}
	}
	break;
	default:
		break;
	}
}

void BoidManager::DetectCollisions(bool everyone)
{
	for (unsigned int i = everyone ? 0: boidID * BOIDS_PER_USER; (int)i < (everyone ? boids.size():(boidID * BOIDS_PER_USER) + BOIDS_PER_USER); i++)
	{
		if (!boids[i]->active)
			continue;
		float currBoidRadius = boids[i]->radius;
		Vector2 currBoidPosition = boids[i]->position;
		
		for (unsigned int j = i + 1 ; (int)j < (everyone ? boids.size() : (boidID * BOIDS_PER_USER) + BOIDS_PER_USER); j++)
		{
			if (!boids[j]->active)
				continue;
			float otherBoidRadius = boids[j]->radius;
			Vector2 otherBoidPosition = boids[j]->position;
			float dstSquared = (float)((otherBoidPosition.x - currBoidPosition.x) * (otherBoidPosition.x - currBoidPosition.x)) +
				(float)((otherBoidPosition.y - currBoidPosition.y) * (otherBoidPosition.y - currBoidPosition.y));
			float radiiSquared = (currBoidRadius + otherBoidRadius) * (currBoidRadius + otherBoidRadius);

			if (dstSquared <= radiiSquared)
			{
				Vector2 dir = otherBoidPosition - currBoidPosition;
				boids[j]->velocity = dir.normalized() * 80;
				dir -= dir * 2;
				boids[i]->velocity = dir.normalized() * 80;
			}
		}
	}
}

void BoidManager::SetBoidActive(int boidIndex, bool active)
{
	if (boidIndex >= 0 && (unsigned)boidIndex < boids.size())
		boids[boidIndex]->active = active;
}
