// NetworkingProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <string.h>
#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/GetTime.h"
#include <vector>
#include <algorithm>

//#define MAX_CLIENTS 10
//#define SERVER_PORT 60000

unsigned int maxClients = 10;
unsigned short serverPort = 60000;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

struct UserInfo
{
	RakNet::SystemAddress userAddress;
	RakNet::RakString username;
};

//Taken From http://www.jenkinssoftware.com/raknet/manual/creatingpackets.html
#pragma pack(push, 1)
struct Package
{
	RakNet::RakString string;
	RakNet::Time timeStamp;
};
#pragma pack(pop)

int main(void)
{
	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	RakNet::Packet* packet;

	printf("What is the Server Port?\n");
	fgets(str, 512, stdin);

	serverPort = atoi(str);
	printf("UserName?\n");
	fgets(str, 512, stdin);
	RakNet::RakString myName;
	myName = str;

	printf("(C) or (S)erver?\n");
	fgets(str, 512, stdin);
	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
	}
	else {
		printf("Max number of Clients?\n");
		fgets(str, 512, stdin);

		maxClients = atoi(str);

		RakNet::SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = true;
	}


	if (isServer)
	{
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(maxClients);
	}
	else {
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 512, stdin);
		if (str[0] == 0) {
			strcpy(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);

	}

	bool connected = false;
	RakNet::SystemAddress addressConnected;
	std::vector<UserInfo> clientsConnected;
	Package* myPackage = new Package();

	while (1)
	{	

		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				printf("Another client has connected.\n");
				UserInfo newUser;
				newUser.userAddress = packet->systemAddress;
				clientsConnected.push_back(newUser);
			}
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");

				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				//RakNet::BitStream bsOut;
				////bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);

				//printf("What's your incoming gamer message?\n");
				//fgets(str, 512, stdin);

				//bsOut.Write(str);
				addressConnected = packet->systemAddress;
				//peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				connected = true;
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("A connection is incoming.\n");
				UserInfo newUser;
				newUser.userAddress = packet->systemAddress;
				clientsConnected.push_back(newUser);
			}
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
					UserDisconnected(packet->systemAddress, clientsConnected);
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
					UserDisconnected(packet->systemAddress, clientsConnected);
				}
				else {
					printf("Connection lost.\n");
				}
				break;
			case ID_GAME_MESSAGE_1:
			{
				
				RakNet::RakString rs;
				RakNet::Time tStamp;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				bsIn.Read(tStamp);
				printf("%s %i\n", rs.C_String(), tStamp);
				if (isServer)
				{
					for(UserInfo user : clientsConnected)
					{
						if (user.userAddress == packet->systemAddress)
							continue;
						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);

						myPackage->string = rs;
						myPackage->timeStamp = RakNet::GetTime();


						bsOut.Write(myPackage->string);
						bsOut.Write(myPackage->timeStamp);
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, user.userAddress, false);
					}
				}
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}

		if (!isServer && connected)
		{
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);

			printf("What's your incoming gamer message?\n");
			fgets(str, 512, stdin);
			myPackage->string = str;
			myPackage->timeStamp = RakNet::GetTime();

			
			bsOut.Write(myPackage->string);
			bsOut.Write(myPackage->timeStamp);
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addressConnected, false);
		}
	}

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}


void UserDisconnected(RakNet::SystemAddress addressDisconnected, std::vector<UserInfo> userList)
{
	std::vector<UserInfo>::iterator it = userList.begin;
	while (it != userList.end)
	{
		if (it->userAddress == addressDisconnected)
		{
			userList.erase(it);
			return;
		};
		it++;
	}
	
}