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
#include <string>

//#define MAX_CLIENTS 10
//#define SERVER_PORT 60000

unsigned int maxClients = 10;
unsigned short serverPort = 60000;




enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_CLIENT_GREETING,
};

#pragma pack(push, 1)
struct UserInfo
{
	RakNet::SystemAddress userAddress;
	char username[512];
};
#pragma pack(pop)
void UserDisconnected(RakNet::SystemAddress addressDisconnected, std::vector<UserInfo*> userList);

#pragma pack(push, 1)
struct UserMessage
{
	int messageId = 0;
	char username[512];
	char message[512];
	char recipient[512];
	bool isPrivate = false;
};
#pragma pack(pop)

//Taken From http://www.jenkinssoftware.com/raknet/manual/creatingpackets.html
#pragma pack(push, 1)
struct Package
{
	char name[512];
	RakNet::RakString string;
	RakNet::Time timeStamp;
};
#pragma pack(pop)

int main(void)
{
	char str[512];
	char userName[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	RakNet::Packet* packet;

	printf("What is the Server Port?\n");
	fgets(str, 512, stdin);

	serverPort = atoi(str);

	printf("(C) or (S)erver?\n");
	fgets(str, 512, stdin);
	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		printf("Enter Username?\n");
		fgets(userName, 512, stdin);	

		int place = ((std::string)userName).find("\n");
		std::string stringuser = ((std::string)userName).substr(0, place);

		strcpy(userName, stringuser.c_str());
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;

	}
	else {
		printf("Max number of Clients?\n");
		fgets(str, 512, stdin);
		if (str[0] == 13 || str[0] == 10)
			maxClients = 12;
		else
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
		if (str[0] == 10) {
			strcpy(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);
		

	}

	bool connected = false;
	RakNet::SystemAddress addressConnected;
	std::vector<UserInfo*> clientsConnected;
	Package* myPackage = new Package();

	char keyInput[512];
	memset(keyInput, 0, 512);
	int keyIndex = 0;
	bool sendingPrivateMessage = false;

	while (1)
	{	
		if (!isServer && connected)
		{			
			
			for (int k = 32; k <= 127; k++)
			{
				if (GetAsyncKeyState(k) & 0x0001)
				{
					printf("%c", k);
					if (keyIndex <= 512)
					{
						keyInput[keyIndex] = k;
						keyIndex++;
					}
				}
			}

			if (GetAsyncKeyState(8) & 0x0001)
			{
				if (keyIndex-1 <= 0)
				{
					keyInput[keyIndex] = 0;
					keyIndex--;
				}
			}
			if (GetAsyncKeyState(13) & 0x0001 || GetAsyncKeyState(10) & 0x0001)
			{
				printf("\n");
				UserMessage myMessage;
				if (sendingPrivateMessage)
				{
					myMessage.isPrivate = true;
					int place  = ((std::string)keyInput).find("POO");
					std::string recip = ((std::string)keyInput).substr(0, place);
					strcpy(myMessage.recipient, recip.c_str());

					std::string mes = ((std::string)keyInput).substr(place+2, 512);
					strcpy(myMessage.message, keyInput);
				}
				else
				{
					myMessage.isPrivate = false;
					strcpy(myMessage.message, keyInput);
				}
				

				myMessage.messageId = ID_GAME_MESSAGE_1;
				strcpy(myMessage.username, userName);
				peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, addressConnected, false);
				keyIndex = 0;
				memset(keyInput,0, 512);

			}

			if (GetAsyncKeyState(9) & 0x0001)
			{
				if (!sendingPrivateMessage)
				{
					sendingPrivateMessage = true;

					printf("Sending Private Message (username:message)?\n");
					memset(keyInput, 0, 512);
				}
				else if (sendingPrivateMessage)
				{
					sendingPrivateMessage = false;
					memset(keyInput, 0, 512);
				}
			}
		}
		/*else if(isServer)
		{
			if (GetAsyncKeyState(13) & 0x0001 || GetAsyncKeyState(10) & 0x0001)
			{
				printf("Users Connected : ");
				for (UserInfo* sa : clientsConnected)
				{
					printf("\nUser : %s, IP : %s", sa->username, sa->userAddress.ToString());
				}
			}
		}*/


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
				UserInfo* newUser = new UserInfo();
				newUser->userAddress = packet->systemAddress;
				clientsConnected.push_back(newUser);
			}
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("Our connection request has been accepted.\n");

					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_CLIENT_GREETING);
					bsOut.Write(userName);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					addressConnected = packet->systemAddress;
					connected = true;
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("A connection is incoming.\n");
				UserInfo* newUser = new UserInfo();
				newUser->userAddress = packet->systemAddress;
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
				
				//RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				UserMessage* incommingMessage = (UserMessage*)packet->data;
				
				if (isServer)
				{
					if (incommingMessage->isPrivate)
					{
						printf("super secret message to %s!", incommingMessage->recipient);

						for (UserInfo* sa : clientsConnected)
						{
							printf("*********%s:%s******\n", sa->username, incommingMessage->recipient);

							std::string name1 = sa->username;
							std::string name2 = incommingMessage->recipient;
							bool matching = std::strcmp(name1.c_str(), name2.c_str());

							if (!matching)
							{
								printf("MATCHING USERNAMES");
								printf("\n %s: %s\n", incommingMessage->username, incommingMessage->message);
								char message[512];
								UserMessage myMessage;
								myMessage.messageId = ID_GAME_MESSAGE_1;
								strcpy(myMessage.username, incommingMessage->username);
								strcpy(myMessage.message, incommingMessage->message);
								peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, sa->userAddress, false);

								break;
							}
							
						}
					}
					else if (incommingMessage->isPrivate == false)
					{
						printf("not that secret message!\n");



						for (UserInfo* sa : clientsConnected)
						{
							if (sa->userAddress == packet->systemAddress)
							{
								printf("\n %s: %s\n", incommingMessage->username, incommingMessage->message);
								continue;
							}
							char message[512];
							UserMessage myMessage;
							myMessage.messageId = ID_GAME_MESSAGE_1;
							strcpy(myMessage.username, incommingMessage->username);
							strcpy(myMessage.message, incommingMessage->message);
							peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, sa->userAddress, false);
						}
					}
				}
				else
				{
					printf("\n %s: %s\n", incommingMessage->username, incommingMessage->message);
				}
			}
			break;
			case ID_CLIENT_GREETING:
			{
				char input[512];
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(input);
				UserInfo user;

				for (UserInfo* sa : clientsConnected)
				{
					if (sa->userAddress == packet->systemAddress)
					{
						
						strcpy(sa->username, input);
						printf("Sending Respone to user.. %s", input);
						UserMessage myMessage;
						memset(myMessage.message, 0, 512);
						myMessage.messageId = ID_GAME_MESSAGE_1;
						strcpy(myMessage.username, "Server");
						strcpy(myMessage.message, "You have connected!");
						peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, sa->userAddress, false);
					}
					else
					{
						char message[512];
						strcpy(message, " has Joined");
						UserMessage myMessage;
						myMessage.messageId = ID_GAME_MESSAGE_1;
						strcpy(myMessage.username, input);
						strcpy(myMessage.message, message);
						peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, sa->userAddress, false);
						
					}
				}
							
			}
			break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}

		
	}

	RakNet::RakPeerInterface::DestroyInstance(peer);
	for (UserInfo* user : clientsConnected)
	{
		delete user;
	}
	clientsConnected.clear();

	return 0;
}


void UserDisconnected(RakNet::SystemAddress addressDisconnected, std::vector<UserInfo*> userList)
{
	std::vector<UserInfo*>::iterator it = userList.begin();
	while (it != userList.end())
	{
		
		if ((*it)->userAddress == addressDisconnected)
		{
			userList.erase(it);
			return;
		};
		it++;
	}
	
}