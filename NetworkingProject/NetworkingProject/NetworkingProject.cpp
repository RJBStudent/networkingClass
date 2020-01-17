// NetworkingProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include "RakNet/RakPeerInterface.h"

//#define MAX_CLIENTS 10
//#define SERVER_PORT 60000

unsigned int maxClients = 10;
unsigned short serverPort = 60000;

int main(void)
{
	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;

	printf("What is the Server Port?\n");
	fgets(str, 512, stdin);

	serverPort = atoi(str);

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


	// TODO - Add code body here

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}