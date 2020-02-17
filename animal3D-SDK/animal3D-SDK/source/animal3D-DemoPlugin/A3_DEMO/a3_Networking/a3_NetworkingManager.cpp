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
a3i32 a3netProcessInbound(a3_NetworkingManager* net, EventManager* events, GameObject* go, a3_ChatManager* chat)
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
						RakNet::BitStream bsOut[1];
						bsOut->Write((RakNet::MessageID)ID_TIMESTAMP);
						bsOut->Write(RakNet::GetTime());
						bsOut->Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut->Write("Hello world");
						peer->Send(bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						net->connectedAddress = &packet->systemAddress;
						// ****TO-DO: write timestamped message
						printf("\n time : %u", (unsigned int)RakNet::GetTime());

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
				case ID_MOVE_MESSAGE:
				{

					MoveMessage* message = (MoveMessage*)packet->data;
					printf("RECIEVED MOVE MESSAGE\n");
					

					if (net->isServer)
					{
						MoveEvent* move = new MoveEvent(message->x, message->y, go, false);
						events->AddEvent(move);
						
						MoveMessage newMessage;
						newMessage.messageId = ID_MOVE_MESSAGE;
						newMessage.x = message->x;
						newMessage.y = message->y;

						RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
						peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
						printf("SENDING OUT\n");
					}
					else
					{
						MoveEvent* move = new MoveEvent(message->x, message->y, go, false);
						events->AddEvent(move);
						printf("ADDING EVENT\n");
					}
				}

					break;
				case ID_CHAT_MESSAGE:
				{
					a3_NetChatMessage* message = (a3_NetChatMessage*)packet->data;
					if (net->isServer)
					{
						a3_NetChatMessage newMessage;
						newMessage.typeID = message->typeID;
						strcpy(newMessage.user, message->user);
						strcpy(newMessage.message, message->message);

						RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
						peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
						printf("SENDING OUT\n");
					}
					else
					{
						AddMessage(chat, *message);
					}
				}
				break;
				case ID_STRING_MESSAGE:
				{

					StringMessage* message = (StringMessage*)packet->data;
					printf("RECIEVED MOVE MESSAGE\n");


					if (net->isServer)
					{
						StringEvent* move = new StringEvent(message->newString, go, false);
						events->AddEvent(move);

						StringMessage newMessage;
						newMessage.messageId = ID_STRING_MESSAGE;
						strcpy(newMessage.newString, message->newString);

						RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
						peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
						printf("SENDING OUT\n");
					}
					else
					{
						StringEvent* move = new StringEvent(message->newString, go, false);
						events->AddEvent(move);
						printf("ADDING EVENT\n");
					}
				}
				case ID_ISRED_MESSAGE:
				{

					RedMessage* message = (RedMessage*)packet->data;
					printf("RECIEVED MOVE MESSAGE\n");


					if (net->isServer)
					{
						BoolEvent* move = new BoolEvent(message->isRed, go, false);
						events->AddEvent(move);

						RedMessage newMessage;
						newMessage.messageId = ID_STRING_MESSAGE;
						newMessage.isRed = message->isRed;

						RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
						peer->Send(reinterpret_cast<char*>(&newMessage), sizeof(newMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
						printf("SENDING OUT\n");
					}
					else
					{
						BoolEvent* move = new BoolEvent(message->isRed, go, false);
						events->AddEvent(move);
						printf("ADDING EVENT\n");
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
