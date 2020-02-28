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

void BoidManager::SpawnNewBoid(Vector2 pos, Vector2 velocity, bool active,  float rotation, int radius)
{
	Boid* newBoid = new Boid(active, pos, velocity, rotation, radius);
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
		if (net->isServer)	
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
	if (boidIndex < 0 || (unsigned)boidIndex >= boids.size())
	{
		boids[boidID]->position.x = x;
		boids[boidID]->position.y = y;
	}
}


void BoidManager::ProcessOutbounds(a3_NetworkingManager* net)
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
				Vector2Message newMessage;
				newMessage.messageId = ID_SET_BOID_POS;
				newMessage.idIndex = i;
				newMessage.xValue = boids[i]->position.x;
				newMessage.yValue = boids[i]->position.y;
				RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
				peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
			}

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
				Vector2Message newMessage;
				newMessage.messageId = ID_SET_BOID_POS;
				newMessage.idIndex = i;
				newMessage.xValue = boids[i]->position.x;
				newMessage.yValue = boids[i]->position.y;
				RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
				RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
				peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);
			}
		}
	}
	break;
	case 3:
	{
		if (net->isServer)
		{
			
				for (unsigned int i = boidID * BOIDS_PER_USER; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++)
				{
					if (i < 0 || i > boids.size())
						continue;
					Vector2Message newMessage;
					newMessage.messageId = ID_SET_BOID_POS;
					newMessage.idIndex = i;
					newMessage.xValue = boids[i]->position.x;
					newMessage.yValue = boids[i]->position.y;
					RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
					peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
				}

			
		}
		else
		{
			for (unsigned int i = boidID * BOIDS_PER_USER; (int)i < (boidID * BOIDS_PER_USER) + BOIDS_PER_USER; i++)
			{
				if (i < 0 || i > boids.size())
					continue;
				Vector2Message newMessage;
				newMessage.messageId = ID_SET_BOID_POS;
				newMessage.idIndex = i;
				newMessage.xValue = boids[i]->position.x;
				newMessage.yValue = boids[i]->position.y;
				RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
				RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
				peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);
			}						
		}
	}
	break;
	default:
		break;
	}
}

void BoidManager::DetectCollisions()
{
	for (unsigned int i = 0; i < boids.size(); i++)
	{
		if (i < 0 || i > boids.size())
			break;
		int currBoidRadius = boids[i]->radius;
		Vector2 currBoidPosition = boids[i]->position;

		for (unsigned int j = i+1; j < boids.size(); j++)
		{
			if (j < 0 || j > boids.size())
				break;
			int otherBoidRadius = boids[j]->radius;
			Vector2 otherBoidPosition = boids[j]->position;
			int dstSquared = (int)((otherBoidPosition.x - currBoidPosition.x) * (otherBoidPosition.x - currBoidPosition.x)) +
				(int)((otherBoidPosition.y - currBoidPosition.y) * (otherBoidPosition.y - currBoidPosition.y));
			int radiiSquared = (currBoidRadius + otherBoidRadius) * (currBoidRadius + otherBoidRadius);

			printf("disSqr:%i radii:%i \n", dstSquared, radiiSquared);
			if (dstSquared < radiiSquared)
			{
				printf("disSqr:%i radii:%i \n", dstSquared, radiiSquared);
				Vector2 dir = otherBoidPosition - currBoidPosition;
				boids[j]->velocity = dir.normalized() * 30;
				dir -= dir * 2;
				boids[i]->velocity = dir.normalized() * 30;
			}
		}
	}
}
