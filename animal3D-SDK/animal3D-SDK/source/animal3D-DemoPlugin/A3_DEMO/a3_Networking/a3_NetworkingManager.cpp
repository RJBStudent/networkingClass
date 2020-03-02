/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_NetworkingManager.cpp
	RakNet networking wrapper.
*/


#include "../a3_NetworkingManager.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/BitStream.h"
#include "RakNet/GetTime.h"
#include "A3_DEMO/a3_ChatManager.h"
#include "A3_DEMO/NetworkMessages.h"
#include "A3_DEMO/EventManager.h"
#include "A3_DEMO/Event.h"
#include "../BoidManager.h"
#include "A3_DEMO/Vector2.h"



//-----------------------------------------------------------------------------
// networking stuff


#pragma pack(push, 1)

struct a3_NetGameMessageData
{
	unsigned char typeID;

	// ****TO-DO: implement game message data struct

};

#pragma pack (pop)


// startup networking
a3i32 a3netStartup(a3_NetworkingManager* net, a3ui16 port_inbound, a3ui16 port_outbound, a3ui16 maxConnect_inbound, a3ui16 maxConnect_outbound)
{
	if (net && !net->peer)
	{
		RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
		if (peer)
		{
			RakNet::SocketDescriptor socket[2] = {
				RakNet::SocketDescriptor(port_inbound, 0),		// server settings
				RakNet::SocketDescriptor(),//port_outbound, 0),	// client settings
			};

			// choose startup settings based on 
			a3boolean const isServer = port_inbound && maxConnect_inbound;
			a3boolean const isClient = port_outbound && maxConnect_outbound;
			if (peer->Startup(maxConnect_inbound + maxConnect_outbound, socket + isClient, isServer + isClient) == RakNet::StartupResult::RAKNET_STARTED)
			{
				peer->SetMaximumIncomingConnections(maxConnect_inbound);
				peer->SetOccasionalPing(true);

				net->port_inbound = port_inbound;
				net->port_outbound = port_outbound;
				net->maxConnect_inbound = maxConnect_inbound;
				net->maxConnect_outbound = maxConnect_outbound;
				net->peer = peer;

				return 1;
			}
		}
	}
	return 0;
}

// shutdown networking
a3i32 a3netShutdown(a3_NetworkingManager* net)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		RakNet::RakPeerInterface::DestroyInstance(peer);
		net->peer = 0;
		return 1;
	}
	return 0;
}


// connect
a3i32 a3netConnect(a3_NetworkingManager* net, a3netAddressStr const ip)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		peer->Connect(ip, net->port_outbound, 0, 0);
		return 1;
	}
	return 0;
}

// disconnect
a3i32 a3netDisconnect(a3_NetworkingManager* net)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		a3ui16 i, j = peer->NumberOfConnections();
		for (i = 0; i < j; ++i)
			peer->CloseConnection(peer->GetSystemAddressFromIndex(i), true);
		return 1;
	}
	return 0;
}


// process inbound packets
a3i32 a3netProcessInbound(a3_NetworkingManager* net, EventManager* events, BoidManager* boidManager, a3_ChatManager* chat)
{
	if (net && net->peer)
	{
		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		RakNet::Packet* packet;
		RakNet::MessageID msg;
		a3i32 count = 0;

		for (packet = peer->Receive();
			packet;
			peer->DeallocatePacket(packet), packet = peer->Receive(), ++count)
		{
			RakNet::BitStream bs_in(packet->data, packet->length, false);
			bs_in.Read(msg);
			switch (msg)
			{
				// check for timestamp and process
			case ID_TIMESTAMP:
				bs_in.Read(msg);
				// ****TO-DO: handle timestamp
				RakNet::Time sendTime, dt, receiveTime;
				bs_in.Read(sendTime);
				receiveTime = RakNet::GetTime();
				dt = receiveTime - sendTime;
				printf("\nsend Time : %u", (unsigned int)sendTime);
				printf("\nRecieved Time : %u", (unsigned int)receiveTime);
				printf("\nLatency: %u", (unsigned int)dt);

				
				
				
				// do not break; proceed to default case to process actual message contents
			default:
				switch (msg)
				{
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					printf("Another client has disconnected.\n");
					break;
				case ID_REMOTE_CONNECTION_LOST:
					printf("Another client has lost the connection.\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					printf("Another client has connected.\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					printf("Our connection request has been accepted.\n");
					{
						// Use a BitStream to write a custom user message
						// Bitstreams are easier to use than sending casted structures, 
						//	and handle endian swapping automatically
						IntMessage connectMessage;
						connectMessage.intValue = net->dataPackageType;
						connectMessage.messageId = ID_CONNECTED_MESSAGE;
						net->connectedAddress = &packet->systemAddress;
						RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
						peer->Send(reinterpret_cast<char*>(&connectMessage), sizeof(connectMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);

						
					}
					break;
				case ID_NEW_INCOMING_CONNECTION:
					printf("A connection is incoming.\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("The server is full.\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					if (net->maxConnect_outbound) {
						printf("A client has disconnected.\n");
					}
					else {
						printf("We have been disconnected.\n");
					}
					break;
				case ID_CONNECTION_LOST:
					if (net->maxConnect_outbound) {
						printf("A client lost the connection.\n");
					}
					else {
						printf("Connection lost.\n");
					}
					break;

				case ID_GAME_MESSAGE_1:
					printf("DEBUG MESSAGE: received packet ID_GAME_MESSAGE_1.\n");
					{
						RakNet::RakString rs;
						bs_in.Read(rs);
						printf("%s\n", rs.C_String());
					}
					break;
				case ID_CONNECTED_MESSAGE:
				{
					IntMessage* message = (IntMessage*)packet->data;
					if (net->isServer)
					{
						IntMessage connectMessage;
						connectMessage.intValue = net->dataPackageType;
						connectMessage.messageId = ID_CONNECTED_MESSAGE;
						peer->Send(reinterpret_cast<char*>(&connectMessage), sizeof(connectMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						IntMessage boidIDMessage;
						net->nextUserID++;
						boidIDMessage.intValue = net->nextUserID;
						boidIDMessage.messageId = ID_SET_BOID_ID;
						peer->Send(reinterpret_cast<char*>(&boidIDMessage), sizeof(boidIDMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					}
					else
					{
						net->dataPackageType = (a3_NetworkingManager::DataPackagingType)message->intValue;
						printf("Data receive/send type is : %i", (int)net->dataPackageType);
					}
				}
				break;
				case ID_SET_BOID_ID:
				{
					IntMessage* message = (IntMessage*)packet->data;
					boidManager->boidID = message->intValue;	

					for (int i = boidManager->boidID * BoidManager::BOIDS_PER_USER; i < (boidManager->boidID *BoidManager::BOIDS_PER_USER) + BoidManager::BOIDS_PER_USER; i++)
						boidManager->SetBoidActive(i);

				}
					break;

				case ID_SET_BOID_POS:
				{
					Vector2Message* message = (Vector2Message*)packet->data;
					if (net->isServer)
					{
						if(net->dataPackageType == net->DATA_COUPLED)
						{
							//Set positions of boids on server side
							for (int i = 0; i < BoidManager::BOIDS_PER_USER; i++)
							{
								boidManager->UpdateSingleBoid(message->idIndex[i], message->xValue[i], message->yValue[i]);
							}
						}
						peer->Send(reinterpret_cast<char*>(message), sizeof(*message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
					}
					else
					{
						//Set positions of boids on client side at message index
						//boidManager->UpdateSingleBoid();
						for (int i = 0; i < BoidManager::BOIDS_PER_USER; i++)
						{
							boidManager->UpdateSingleBoid(message->idIndex[i], message->xValue[i], message->yValue[i]);
						}
					}
				}
					break;
				default:
					printf("Message with identifier %i has arrived.\n", msg);
					break;
				}
				break;
			}
		}
		return count;
	}
	return 0;
}

// process outbound packets
a3i32 a3netProcessOutbound(a3_NetworkingManager* net)
{
	if (net && net->peer)
	{
		
	}
	return 0;
}


//-----------------------------------------------------------------------------
