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

	a3_demo_callbacks.c/.cpp
	Main implementation of window callback hooks.

	********************************************
	*** THIS IS THE LIBRARY'S CALLBACKS FILE ***
	*** App hooks your demo via this file.   ***
	********************************************
*/


#include "a3_dylib_config_export.h"
#include "a3_DemoState.h"

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"
#include "animal3D-A3DM/animal3D-A3DM.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "a3_Networking/a3_Networking_gs_tictactoe.c"
#include <string>
#include "ButtonObject.h"
#include <vector>

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_CLIENT_GREETING,
	TIC_TAC_TOE_MESSAGE,
	JOIN_GAME

};

#pragma pack(push, 1)
struct UserMessage
{
	int messageId = 0;
	char username[512];
	char message[512];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TicTacToeMessage
{
	int messageId = 0;
	bool isO;
	int xPos;
	int yPos;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct JoinGameRequestMessage
{
	int messageId = 0;
	char username[512];
};
#pragma pack(pop)

struct PlayerData
{
	RakNet::SystemAddress address;
	char username[512];
};

const int MAX_CLIENTS = 12;

struct a3_NetworkState
{

	a3_DemoState demoState[1];
	//---------------------------------------------------------------------
	// general variables pertinent to the state

	

	//Networking stuff
	a3_Timer renderTimer[1];
	RakNet::RakPeerInterface* peer;
	unsigned short serverPort;
	unsigned int maxClients;
	bool isClient;
	bool connected;
	char username[500];
	RakNet::SystemAddress connectedAddres;



	enum GameState
	{
		NO_STATE = 0,
		ENTER_PORT,
		ENTER_USERNAME,
		START_SERVER_OR_CLIENT,
		ENTER_SERVER_IP,
		ENTER_MAX_CLIENTS,
		PICK_GAME,
		JOIN_GAME,
		SELECT_PLAYERS,
		CHALLENGER_YOUR_TURN,
		CHALLENGER_TURN,
		WAITING_TO_JOIN_GAME,
		SPECTATOR,
		CHAT,
		GAMESTATE_COUNT

	};


	int currentPlayerIndex = 0;
	PlayerData players[2];
	std::vector<PlayerData> playersRequestingPlay;

	char textInput[500];
	int inputIndex;

	GameState a3GameState;
	GameState a3LastGameState;

	ButtonObject button[3][3];

	ButtonObject chatButton[1];
	ButtonObject selectUserButtons[MAX_CLIENTS];
	std::vector<UserMessage> messageList;

	bool ticTacToe;
	gs_tictactoe ticTacToe_instance;
	char board[500];
	
};


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------
// miscellaneous functions

// get the size of the persistent state to allocate
//	(good idea to keep it relatively constant, so that addresses don't change 
//	when the library is reloaded... that would mess everything up!)
inline a3ui32 a3demo_getPersistentStateSize()
{
	const a3ui32 minimum = sizeof(a3_NetworkState);
	a3ui32 size = 1;
	while (size < minimum)
		size += size;
	return size;
}


// consistent text initialization
inline void a3demo_initializeText(a3_TextRenderer* text)
{
	a3textInitialize(text, 18, 1, 0, 0, 0);
}


//-----------------------------------------------------------------------------
// callback sub-routines

inline void a3demoCB_keyCharPress_main(a3_NetworkState* demoState, a3i32 asciiKey,
	const a3ui32 demoSubMode, const a3ui32 demoOutput,
	const a3ui32 demoSubModeCount, const a3ui32 demoOutputCount)
{
	
}

inline void a3demoCB_keyCharHold_main(a3_NetworkState* demoState, a3i32 asciiKey)
{
	// handle special cases immediately
	switch (asciiKey)
	{
	case 'l':
		break;
	case 'L':
		break;
	}
}

#include "GL/glew.h"

void a3demoTestRender(a3_NetworkState const* demoState)
{
	//Clear color
	glClear(GL_COLOR_BUFFER_BIT);
	
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, 0, 0, demoState->demoState->windowWidth, demoState->demoState->windowHeight);

	const a3_DemoStateShaderProgram* currentDemoProgram = demoState->demoState->prog_drawTexture;
	a3shaderProgramActivate(currentDemoProgram->program);
	
	a3mat4 projMat;
	a3real4x4MakeOrthographicProjectionPlanes(projMat.m, 0, (a3real)demoState->demoState->windowWidth, 0, (a3real)demoState->demoState->windowHeight, 0, 0, 1);
	//Button Render
	//{
	if (demoState->connected) {
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//demoState->button->Render(demoState->demoState, currentDemoProgram, projMat);
				demoState->button[i][j].Render(demoState->demoState, currentDemoProgram, projMat);
			}
		}

		demoState->chatButton->Render(demoState->demoState, currentDemoProgram, projMat);
	}

	if (demoState->a3GameState == a3_NetworkState::GameState::SELECT_PLAYERS)
	{
		int count = 0;
		for (PlayerData pd : demoState->playersRequestingPlay)
		//while(count < MAX_CLIENTS)
		{
			printf("%i", count);
			demoState->selectUserButtons[count].Render(demoState->demoState, currentDemoProgram, projMat);
				count++;
		}
	}

	a3textureDeactivate(a3tex_unit00);
	a3shaderProgramDeactivate();
	//}

	//draw some text
	switch (demoState->a3GameState)
	{
	case a3_NetworkState::GameState::ENTER_PORT:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter Server Port:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::ENTER_USERNAME:
	{

		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter Username:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::START_SERVER_OR_CLIENT:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter (S) for Server or (c) for client:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::ENTER_SERVER_IP:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter Server IP:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::ENTER_MAX_CLIENTS:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter Max Clients:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::PICK_GAME:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "TicTacToe(T) or BattleShip(B):  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::JOIN_GAME:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Join Game:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::SELECT_PLAYERS:
	{
		a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "Select Players:  %s", demoState->textInput);

		a3real xP = 0.4f;
		int count = 0;
		a3real yP = 0.7f;
		for (PlayerData pd : demoState->playersRequestingPlay)
		{
		a3textDraw(demoState->demoState->text, xP, yP, -1, 0, 0, 0, 1, "%s", pd.username);
		xP += (count % 2) * 90;
		yP += (count % 2) * 60;
		count++;
		}
	}
	break;
	case a3_NetworkState::GameState::CHALLENGER_TURN:
	{
		//Render Challenger OPtions
		//a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter Max Clients:  %s", demoState->textInput);
		if (demoState->ticTacToe)
		{
			a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "%s   %s", demoState->board, demoState->textInput);

		}
		a3textDraw(demoState->demoState->text, 0.4f, 0.8f, -1, 0, 0, 0, 1, "Chat");
	}
	break;
	case a3_NetworkState::GameState::CHALLENGER_YOUR_TURN:
	{
		//Render Challenger OPtions
		//a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter Max Clients:  %s", demoState->textInput);
		if (demoState->ticTacToe)
		{
			a3textDraw(demoState->demoState->text, -1, 0, -1, 1, 1, 1, 1, "%s   %s", demoState->board, demoState->textInput);

		}
		a3textDraw(demoState->demoState->text, 0.4f, 0.8f, -1, 0, 0, 0, 1, "Chat");
	}
	break;
	case a3_NetworkState::GameState::SPECTATOR:
	{
		//Render Spectator Options
		//a3textDraw(demoState->text, -1, -1, -1, 1, 1, 1, 1, "Enter Max Clients:  %s", demoState->textInput);
	}
	break;
	case a3_NetworkState::GameState::CHAT:
	{
		a3real val = 0.9f;
		

		//Render Chat
		for ( UserMessage um : demoState->messageList)
		{
			a3textDraw(demoState->demoState->text, -1, val, -1, 1, 1, 1, 1, "%s : %s", um.username, um.message);
			val -= 0.1f;
		}

		a3textDraw(demoState->demoState->text, -1, -1, -1, 1, 1, 1, 1, "Chat Message:  %s", demoState->textInput);
		a3textDraw(demoState->demoState->text, 0.35f, 0.8f, -1, 0, 0, 0, 1, "Exit Chat");
	}
	break;
	default:
	{}
	}

}


void a3demoTestNetworking_Receive(a3_NetworkState*  demoState)
{
	//raknet business
	RakNet::Packet* packet;
	for (packet = demoState->peer->Receive(); packet; demoState->peer->DeallocatePacket(packet), packet = demoState->peer->Receive())
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
			/*UserInfo* newUser = new UserInfo();
			newUser->userAddress = packet->systemAddress;
			clientsConnected.push_back(newUser);*/
		}
		break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Our connection request has been accepted.\n");
			demoState->connectedAddres = packet->systemAddress;
			
			UserMessage myMessage;

			strcpy(myMessage.message, "");
			myMessage.messageId = ID_CLIENT_GREETING;
			strcpy(myMessage.username, demoState->username);
			demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage),
				HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->connectedAddres, false);
			
			demoState->connected = true;
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
		{
			printf("A connection is incoming.\n");
			/*UserInfo* newUser = new UserInfo();
			newUser->userAddress = packet->systemAddress;
			clientsConnected.push_back(newUser);*/
		}
		break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:

			if (!demoState ->isClient) {
				printf("A client has disconnected.\n");
				//UserDisconnected(packet->systemAddress, clientsConnected);
			}
			else {
				printf("We have been disconnected.\n");
			}

			break;
		case ID_CONNECTION_LOST:
			if (!demoState->isClient) {
				printf("A client lost the connection.\n");
				//UserDisconnected(packet->systemAddress, clientsConnected);
			}
			else {
				printf("Connection lost.\n");
			}
			break;
		case ID_GAME_MESSAGE_1:
		{

			UserMessage* incommingMessage = (UserMessage*)packet->data;

			if (!demoState->isClient)
			{
				printf("\n %s: %s\n", incommingMessage->username, incommingMessage->message);
				UserMessage myMessage;
				myMessage.messageId = ID_GAME_MESSAGE_1;
				strcpy(myMessage.username, incommingMessage->username);
				strcpy(myMessage.message, incommingMessage->message);
				demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

			}

			else
			{
				printf("\n %s: %s\n", incommingMessage->username, incommingMessage->message);
			}

			demoState->messageList.push_back(*incommingMessage);
			
		}
		break;
		case ID_CLIENT_GREETING:
		{
			UserMessage* incommingMessage = (UserMessage*)packet->data;			
						
			printf("Sending Respone to user.. %s", incommingMessage->username);
			UserMessage myMessage;
			memset(myMessage.message, 0, 512);
			myMessage.messageId = ID_GAME_MESSAGE_1;
			strcpy(myMessage.username, "Server");
			strcpy(myMessage.message, "You have connected!");
			demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		
			char message[512];
			strcpy(message, " has Joined");						
			strcpy(myMessage.username, incommingMessage->username);
			strcpy(myMessage.message, message);
			demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

				
			

		}
		break;
		case TIC_TAC_TOE_MESSAGE:
		{

		}
		break;
		case JOIN_GAME:
		{
			JoinGameRequestMessage* incommingMessage = (JoinGameRequestMessage*)packet->data;
			PlayerData newPlayer;
			newPlayer.address = packet->systemAddress;			
			strcpy(newPlayer.username, incommingMessage->username);
			demoState->playersRequestingPlay.push_back(newPlayer);
		}
		break;
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}

}

void a3demoTestNetworking_Send(a3_NetworkState const* demoState)
{
	//raknet business

}

void a3demoTestInput(a3_NetworkState* demoState, char(&input)[500], int& index, char(&board)[500])
{
	for (int i = 32; i < 127; i++)
	{
		if (demoState->demoState->keyboard->keyASCII.key[i] && !demoState->demoState->keyboard->keyASCII0.key[i])
		{
			if (demoState->inputIndex <= 500)
			{

				input[demoState->inputIndex] = i;
				index++;
			}
		}
	}

	if (demoState->demoState->keyboard->keyASCII.key[8]&& !demoState->demoState->keyboard->keyASCII0.key[8])
	{
		if (demoState->inputIndex > 0)
		{

			input[demoState->inputIndex] = 0;
			index--;
		}
		else if (demoState->inputIndex == 0)
		{
			input[demoState->inputIndex] = 0;
		}
	}

	//Enter
	if ((demoState->demoState->keyboard->key.key[10] && !demoState->demoState->keyboard->key0.key[10]) ||
		(demoState->demoState->keyboard->key.key[13] && !demoState->demoState->keyboard->key0.key[13]))
	{


		switch (demoState->a3GameState)
		{
			case a3_NetworkState::GameState::ENTER_PORT:
			{
				demoState->serverPort = atoi(input);
				demoState->a3GameState = a3_NetworkState::START_SERVER_OR_CLIENT;
			}
			break;
			case a3_NetworkState::GameState::ENTER_USERNAME:
			{
				strcpy(demoState->username, input);
				demoState->a3GameState = a3_NetworkState::ENTER_SERVER_IP;
			}
			break;
			case a3_NetworkState::GameState::START_SERVER_OR_CLIENT:
			{
				if (input[0] == 'c' || input[0] == 'C')
				{
					RakNet::SocketDescriptor sd;
					demoState->peer->Startup(1, &sd, 1);
					demoState->isClient = true;
					demoState->a3GameState = a3_NetworkState::ENTER_USERNAME;
				}
				else
				{
					demoState->a3GameState = a3_NetworkState::ENTER_MAX_CLIENTS;
				}
			}
			break;
			case a3_NetworkState::GameState::ENTER_SERVER_IP:
			{

				if (input[0] == 0)
				{
					strcpy(input, "127.0.0.1");
				}
				demoState->peer->Connect(input, demoState->serverPort, 0, 0);
				demoState->a3GameState = a3_NetworkState::JOIN_GAME;				
			}
			break;
			case a3_NetworkState::GameState::ENTER_MAX_CLIENTS:
			{
				demoState->maxClients = atoi(input);
				if (demoState->maxClients < 3)
					demoState->maxClients = 3;
				else if(demoState->maxClients > MAX_CLIENTS)
					 demoState->maxClients = MAX_CLIENTS;


				RakNet::SocketDescriptor sd(demoState->serverPort, 0);
				demoState->peer->Startup(demoState->maxClients, &sd, 1);
				demoState->isClient = false; 
				printf("Starting the server.\n");
				demoState->peer->SetMaximumIncomingConnections(demoState->maxClients);
				demoState->a3GameState = a3_NetworkState::PICK_GAME;
				demoState->connected = true;
			}
			break;
			case a3_NetworkState::GameState::PICK_GAME:
			{
				if (input[0] == 't' || input[0] == 'T')
				{
					demoState->ticTacToe = true;
					launchTicTacToe(demoState->ticTacToe_instance);
				}
				else
				{
					demoState->ticTacToe = false;
				}
				demoState->a3GameState = a3_NetworkState::SELECT_PLAYERS;
			}
			break;
			case a3_NetworkState::GameState::SELECT_PLAYERS:
			{
				//SHOW ALL PLAYERS AS BUTTONS
			}
			break;
			case a3_NetworkState::GameState::JOIN_GAME:
			{
				if (input[0] == 's' || input[0] == 'S')
				{
					demoState->a3GameState= a3_NetworkState::GameState::SPECTATOR;
				}
				else
				{
					JoinGameRequestMessage myMessage;
					myMessage.messageId = JOIN_GAME;
					strcpy(myMessage.username, demoState->username);
					demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), 
						HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->connectedAddres,false);


				}

			}
			break;
			case a3_NetworkState::GameState::CHALLENGER_TURN:
			{
			}
			break;
			case a3_NetworkState::GameState::CHAT:
			{
				
					UserMessage myMessage;

					strcpy(myMessage.message, input);
					myMessage.messageId = ID_GAME_MESSAGE_1;
					strcpy(myMessage.username, demoState->username);					
					demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage), 
						HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->connectedAddres, !demoState->isClient);
				
			}
			break;
			case a3_NetworkState::GameState::CHALLENGER_YOUR_TURN:
			{
				if (demoState->ticTacToe)
				{
					bool madeMove = false;
					if (input[0] == 'O' || input[0] == 'o')
					{
						gs_tictactoe_setSpaceState(demoState->ticTacToe_instance, gs_tictactoe_space_state::gs_tictactoe_space_o, (int)input[1], (int)input[2]);
						madeMove = true;
						printf("O move\n");
					}
					else if (input[0] == 'X' || input[0] == 'x')
					{
						gs_tictactoe_setSpaceState(demoState->ticTacToe_instance, gs_tictactoe_space_state::gs_tictactoe_space_x, (int)input[1], (int)input[2]);
						madeMove = true;
						printf("X move\n");
					}
					if (madeMove)
					{
						std::fill(board, board + 500, '.');
						int boardIndex = 0;
						for (int i = 0; i < 3; i++)
						{
							for (int j = 0; j < 3; j++)
							{
								gs_tictactoe_space_state spaceState = gs_tictactoe_getSpaceState(demoState->ticTacToe_instance, i, j);
								if (spaceState == gs_tictactoe_space_state::gs_tictactoe_space_open) {
									board[boardIndex] = '*';
								}
								else if (spaceState == gs_tictactoe_space_state::gs_tictactoe_space_o) {
									board[boardIndex] = 'O';
								}
								else if (spaceState == gs_tictactoe_space_state::gs_tictactoe_space_x) {
									board[boardIndex] = 'X';
								}
								boardIndex++;
							}
							board[boardIndex] = '|';
							boardIndex++;
						}
					}
					
				}
			}
			break;
			default:
			{}
		}
		memset(input, 0, 500);
		index = 0;
	}

	//do Input
	if (a3mouseIsPressed(demoState->demoState->mouse, a3mouse_left) )
	{
		if (demoState->a3GameState == a3_NetworkState::GameState::SELECT_PLAYERS)
		{
			int count = 0;
			for (PlayerData pd : demoState->playersRequestingPlay)
			{
				if (demoState->selectUserButtons[count].ButtonClickCheck(demoState->demoState->mouse->x,
					(a3i32)((a3real)demoState->demoState->windowHeight * (1.0 - ((a3real)demoState->demoState->mouse->y / (a3real)demoState->demoState->windowHeight)))))
				{
					demoState->players[demoState->currentPlayerIndex] = pd;
					demoState->currentPlayerIndex++;
				}
				count++;
			}
		}


		if (!demoState->connected)
			return;
		/*if (demoState->button->ButtonClickCheck(demoState->demoState->mouse->x,
			(a3i32) ((a3real)demoState->demoState->windowHeight *(1.0-((a3real)demoState->demoState->mouse->y/ (a3real)demoState->demoState->windowHeight)))))
		{
			printf("Pressed!!!");
		}*/
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//demoState->button->Init(demoState->demoState->tex_earth_dm, 300, 300, 100, 100);
				if (demoState->button[i][j].ButtonClickCheck(demoState->demoState->mouse->x,
					(a3i32)((a3real)demoState->demoState->windowHeight * (1.0 - ((a3real)demoState->demoState->mouse->y / (a3real)demoState->demoState->windowHeight)))))
				{
					if (gs_tictactoe_getSpaceState(demoState->ticTacToe_instance, i, j) == gs_tictactoe_space_state::gs_tictactoe_space_open)
					{
						gs_tictactoe_setSpaceState(demoState->ticTacToe_instance, gs_tictactoe_space_state::gs_tictactoe_space_x, i, j);
						if (gs_tictactoe_getSpaceState(demoState->ticTacToe_instance, i, j) == gs_tictactoe_space_state::gs_tictactoe_space_x)
						{
							demoState->button[i][j].SetTexture(demoState->demoState->tex_skybox_clouds);

							UserMessage myMessage;

							std::string gameMove = "GAMEMOVE" + std::to_string(i) + std::to_string(j) + "X";
							strcpy(myMessage.message, gameMove.c_str());

							myMessage.messageId = ID_GAME_MESSAGE_1;
							strcpy(myMessage.username, demoState->username);
							demoState->peer->Send(reinterpret_cast<char*>(&myMessage), sizeof(myMessage),
								HIGH_PRIORITY, RELIABLE_ORDERED, 0, demoState->connectedAddres, !demoState->isClient);
						}
					}
					
				}
			}
		}

		if (demoState->chatButton->ButtonClickCheck(demoState->demoState->mouse->x,
			(a3i32)((a3real)demoState->demoState->windowHeight * (1.0 - ((a3real)demoState->demoState->mouse->y / (a3real)demoState->demoState->windowHeight)))))
		{
			if (demoState->a3GameState != a3_NetworkState::GameState::CHAT)
			{
				demoState->a3LastGameState = demoState->a3GameState;
				demoState->a3GameState = a3_NetworkState::GameState::CHAT;
			}
			else
			{
				demoState->a3GameState = demoState->a3LastGameState;
			}
		}
	}
}

void a3demoTestUpdate(a3_NetworkState const* demoState)
{
	//Update
	
}

//-----------------------------------------------------------------------------
// callback prototypes
// NOTE: do not move to header; they should be private to this file
// NOTE: you may name these functions whatever you like, just be sure to 
//	update your debug config file: 
//	"<root>/resource/animal3D-data/animal3D-demoinfo-debug.txt"
// copy this config line and the DLL to your main config with a new name when 
//	you're happy with it: 
//	"<root>/resource/animal3D-data/animal3D-demoinfo.txt"


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	A3DYLIBSYMBOL a3_NetworkState* a3demoCB_load(a3_NetworkState* demoState, a3boolean hotbuild);
	A3DYLIBSYMBOL a3_NetworkState* a3demoCB_unload(a3_NetworkState* demoState, a3boolean hotbuild);
	A3DYLIBSYMBOL a3i32 a3demoCB_display(a3_NetworkState* demoState);
	A3DYLIBSYMBOL a3i32 a3demoCB_idle(a3_NetworkState* demoState);
	A3DYLIBSYMBOL void a3demoCB_windowActivate(a3_NetworkState* demoState);
	A3DYLIBSYMBOL void a3demoCB_windowDeactivate(a3_NetworkState* demoState);
	A3DYLIBSYMBOL void a3demoCB_windowMove(a3_NetworkState* demoState, a3i32 newWindowPosX, a3i32 newWindowPosY);
	A3DYLIBSYMBOL void a3demoCB_windowResize(a3_NetworkState* demoState, a3i32 newWindowWidth, a3i32 newWindowHeight);
	A3DYLIBSYMBOL void a3demoCB_keyPress(a3_NetworkState* demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyHold(a3_NetworkState* demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyRelease(a3_NetworkState* demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyCharPress(a3_NetworkState* demoState, a3i32 asciiKey);
	A3DYLIBSYMBOL void a3demoCB_keyCharHold(a3_NetworkState* demoState, a3i32 asciiKey);
	A3DYLIBSYMBOL void a3demoCB_mouseClick(a3_NetworkState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseDoubleClick(a3_NetworkState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseRelease(a3_NetworkState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseWheel(a3_NetworkState* demoState, a3i32 delta, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseMove(a3_NetworkState* demoState, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseLeave(a3_NetworkState* demoState);

#ifdef __cplusplus
}
#endif	// __cplusplus


//-----------------------------------------------------------------------------
// callback implementations

// demo is loaded
A3DYLIBSYMBOL a3_NetworkState* a3demoCB_load(a3_NetworkState* demoState, a3boolean hotbuild)
{
	const a3ui32 stateSize = a3demo_getPersistentStateSize();
	const a3ui32 trigSamplesPerDegree = 4;

	// do any re-allocation tasks
	if (demoState && hotbuild)
	{


		const a3ui32 stateSize = a3demo_getPersistentStateSize();
		a3_NetworkState copy = *demoState;

		// example 1: copy memory directly
		free(demoState);
		demoState = (a3_NetworkState*)malloc(stateSize);
		memset(demoState, 0, stateSize);
		*demoState = copy;
		a3trigInitSetTables(trigSamplesPerDegree, demoState->demoState->trigTable);
		
				// call refresh to re-link pointers in case demo state address changed
				a3demo_refresh(demoState->demoState);
				a3demo_initSceneRefresh(demoState->demoState);
				
	}

	// do any initial allocation tasks
	else
	{
		// HEAP allocate persistent state
		// stack object will be deleted at the end of the function
		// good idea to set the whole block of memory to zero
		demoState = (a3_NetworkState*)malloc(stateSize);
		memset(demoState, 0, stateSize);

		// set up trig table (A3DM)
		a3trigInit(trigSamplesPerDegree, demoState->demoState->trigTable);

		// initialize state variables
		// e.g. timer, thread, etc.
		a3timerSet(demoState->renderTimer, 30.0);
		a3timerStart(demoState->renderTimer);
		// text
		a3demo_initializeText(demoState->demoState->text);
		demoState->demoState->textInit = 1;
		demoState->demoState->textMode = 1;
		demoState->demoState->textModeCount = 3;	// 0=off, 1=controls, 2=data
		a3fileStreamMakeDirectory("./data");

		if (!demoState->peer)
		{
			demoState->peer = RakNet::RakPeerInterface::GetInstance();
			if (demoState->peer)
			{				
			}
		}

		demoState->a3GameState = a3_NetworkState::ENTER_PORT;

	


		// enable asset streaming between loads
		//demoState->demoState->streaming = 1;


		// create directory for data


		// set default GL state
		a3demo_setDefaultGraphicsState();

		// geometry
		a3demo_loadGeometry(demoState->demoState);

		// shaders
		a3demo_loadShaders(demoState->demoState);

		// textures
		a3demo_loadTextures(demoState->demoState);

		// scene objects
		a3demo_initScene(demoState->demoState);


		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//demoState->button->Init(demoState->demoState->tex_earth_dm, 300, 300, 100, 100);
				demoState->button[i][j].Init(demoState->demoState->tex_earth_dm, a3real(300 + i*80), a3real(300 + j * 80), 40, 40);
			}
		}
		demoState->chatButton->Init(demoState->demoState->tex_checker, 700, 500, 50, 25);
		
		int i = 0;
		
		for (int j = 0; j < MAX_CLIENTS; j++)
		{
			i += (j % 2) * 60;

			demoState->selectUserButtons[j].Init(demoState->demoState->tex_checker,
				a3real(600 + (j%2) * 90), a3real(10 + i), 40, 25);
		}
		
	}

	// return persistent state pointer

	return demoState;
}

// demo is unloaded; option to unload to prep for hotbuild
A3DYLIBSYMBOL a3_NetworkState* a3demoCB_unload(a3_NetworkState* demoState, a3boolean hotbuild)
{
	// release things that need releasing always, whether hotbuilding or not
	// e.g. kill thread
	// nothing in this example, but then...

	// release persistent state if not hotbuilding
	// good idea to release in reverse order that things were loaded...
	//	...normally; however, in this case there are reference counters 
	//	to make sure things get deleted when they need to, so there is 
	//	no need to reverse!
	if (!hotbuild)
	{
		// free fixed objects
		
		// free graphics objects
		a3demo_unloadGeometry(demoState->demoState);
		a3demo_unloadShaders(demoState->demoState);
		a3demo_unloadTextures(demoState->demoState);

		// validate unload
		a3demo_validateUnload(demoState->demoState);
		
		a3textRelease(demoState->demoState->text);
		// erase other stuff
		a3trigFree();
		if (demoState->peer)
		{
			RakNet::RakPeerInterface::DestroyInstance(demoState->peer);
			demoState->peer = 0;
		}
		// erase persistent state
		free(demoState);
		demoState = 0;
	}

	// return state pointer
	return demoState;
}

// window updates display
// **NOTE: DO NOT USE FOR RENDERING**
A3DYLIBSYMBOL a3i32 a3demoCB_display(a3_NetworkState* demoState)
{
	// do nothing, should just return 1 to indicate that the 
	//	window's display area is controlled by the demo
	return 1;
}

// window idles
A3DYLIBSYMBOL a3i32 a3demoCB_idle(a3_NetworkState* demoState)
{
	// perform any idle tasks, such as rendering
	if (!demoState->demoState->exitFlag)
	{
		if (a3timerUpdate(demoState->renderTimer) > 0)
		{
			// render timer ticked, update demo state and draw
			//a3demo_update(demoState, demoState->renderTimer->secondsPerTick);
			//a3demo_input(demoState, demoState->renderTimer->secondsPerTick);
			//a3demo_render(demoState);

			a3demoTestInput(demoState, demoState->textInput, demoState->inputIndex, demoState->board);//Test GameLogic
			//Send
			a3demoTestNetworking_Send(demoState);
			a3demoTestUpdate(demoState);//Update Game Values
			//Receive
			a3demoTestNetworking_Receive(demoState);
			a3demoTestRender(demoState);//RenderText


			// update input
			a3mouseUpdate(demoState->demoState->mouse);
			a3keyboardUpdate(demoState->demoState->keyboard);
			a3XboxControlUpdate(demoState->demoState->xcontrol);


			// render occurred this idle: return +1
			return +1;
		}

		// nothing happened this idle: return 0
		return 0;
	}

	// demo should exit now: return -1

	return -1;
}

// window gains focus
A3DYLIBSYMBOL void a3demoCB_windowActivate(a3_NetworkState* demoState)
{
	// nothing really needs to be done here...
	//	but it's here just in case
}

// window loses focus
A3DYLIBSYMBOL void a3demoCB_windowDeactivate(a3_NetworkState* demoState)
{
	// reset input; it won't track events if the window is inactive, 
	//	active controls will freeze and you'll get strange behaviors
	a3keyboardReset(demoState->demoState->keyboard);
	a3mouseReset(demoState->demoState->mouse);
	a3XboxControlReset(demoState->demoState->xcontrol);
	a3XboxControlSetRumble(demoState->demoState->xcontrol, 0, 0);
}

// window moves
A3DYLIBSYMBOL void a3demoCB_windowMove(a3_NetworkState* demoState, a3i32 newWindowPosX, a3i32 newWindowPosY)
{
	// nothing needed here
}

// window resizes
A3DYLIBSYMBOL void a3demoCB_windowResize(a3_NetworkState* demoState, a3i32 newWindowWidth, a3i32 newWindowHeight)
{
	a3ui32 i;
	a3_DemoProjector *camera;

	// account for borders here
	const a3i32 frameBorder = 0;
	const a3ui32 frameWidth = newWindowWidth + frameBorder + frameBorder;
	const a3ui32 frameHeight = newWindowHeight + frameBorder + frameBorder;
	const a3real windowAspect = (a3real)newWindowWidth / (a3real)newWindowHeight;
	const a3real frameAspect = (a3real)frameWidth / (a3real)frameHeight;

	// copy new values to demo state
	demoState->demoState->windowWidth = newWindowWidth;
	demoState->demoState->windowHeight = newWindowHeight;
	demoState->demoState->windowWidthInv = a3recip((a3real)newWindowWidth);
	demoState->demoState->windowHeightInv = a3recip((a3real)newWindowHeight);
	demoState->demoState->windowAspect = windowAspect;
	demoState->demoState->frameWidth = frameWidth;
	demoState->demoState->frameHeight = frameHeight;
	demoState->demoState->frameWidthInv = a3recip((a3real)frameWidth);
	demoState->demoState->frameHeightInv = a3recip((a3real)frameHeight);
	demoState->demoState->frameAspect = frameAspect;
	demoState->demoState->frameBorder = frameBorder;
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, -frameBorder, -frameBorder, demoState->demoState->frameWidth, demoState->demoState->frameHeight);
	
	// framebuffers should be initialized or re-initialized here
	//	since they are likely dependent on the window size


	// use framebuffer deactivate utility to set viewport

	// viewing info for projection matrix
	// initialize cameras dependent on viewport
	for (i = 0, camera = demoState->demoState->projector + i; i < demoStateMaxCount_cameraObject; ++i, ++camera)
	{
		camera->aspect = frameAspect;
		a3demo_updateProjectorProjectionMat(camera);
	}
}

// any key is pressed
A3DYLIBSYMBOL void a3demoCB_keyPress(a3_NetworkState* demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_down);
}

// any key is held
A3DYLIBSYMBOL void a3demoCB_keyHold(a3_NetworkState* demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_down);
}

// any key is released
A3DYLIBSYMBOL void a3demoCB_keyRelease(a3_NetworkState* demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_up);
}

// ASCII key is pressed (immediately preceded by "any key" pressed call above)
// NOTE: there is no release counterpart
A3DYLIBSYMBOL void a3demoCB_keyCharPress(a3_NetworkState* demoState, a3i32 asciiKey)
{
	a3keyboardSetStateASCII(demoState->demoState->keyboard, (a3byte)asciiKey);
	/*a3ui32 demoSubMode = demoState->demoSubMode[demoState->demoMode];
	const a3ui32 demoSubModeCount = demoState->demoSubModeCount[demoState->demoMode];
	const a3ui32 demoOutput = demoState->demoOutputMode[demoState->demoMode][demoSubMode];
	const a3ui32 demoOutputCount = demoState->demoOutputCount[demoState->demoMode][demoSubMode];

	// persistent state update

	// handle special cases immediately
	switch (asciiKey)
	{
		// uncomment to make escape key kill the current demo
		// if disabled, use 'exit demo' menu option
//	case 27:
//		demoState->exitFlag = 1;
//		break;

		// reload (T) or toggle (t) text
	case 'T':
		if (!a3textIsInitialized(demoState->text))
		{
			a3demo_initializeText(demoState->text);
			demoState->textInit = 1;
		}
		else
		{
			a3textRelease(demoState->text);
			demoState->textInit = 0;
		}
		break;
	case 't':
		demoState->textMode = (demoState->textMode + 1) % demoState->textModeCount;
		break;

		// reload all shaders in real-time
	case 'P':
		a3demo_unloadShaders(demoState);
		a3demo_loadShaders(demoState);
		break;


		// change pipeline mode
	case '.':
		demoState->demoMode = (demoState->demoMode + 1) % demoState->demoModeCount;
		break;
	case ',':
		demoState->demoMode = (demoState->demoMode + demoState->demoModeCount - 1) % demoState->demoModeCount;
		break;

		// change pipeline stage
	case '>':
		demoSubMode = demoState->demoSubMode[demoState->demoMode] = (demoSubMode + 1) % demoSubModeCount;
		break;
	case '<':
		demoSubMode = demoState->demoSubMode[demoState->demoMode] = (demoSubMode + demoSubModeCount - 1) % demoSubModeCount;
		break;

		// change stage output
	case '}':
		demoState->demoOutputMode[demoState->demoMode][demoSubMode] = (demoOutput + 1) % demoOutputCount;
		break;
	case '{':
		demoState->demoOutputMode[demoState->demoMode][demoSubMode] = (demoOutput + demoOutputCount - 1) % demoOutputCount;
		break;


		// toggle grid
	case 'g':
		demoState->displayGrid = 1 - demoState->displayGrid;
		break;

		// toggle world axes
	case 'x':
		demoState->displayWorldAxes = 1 - demoState->displayWorldAxes;
		break;

		// toggle object axes
	case 'z':
		demoState->displayObjectAxes = 1 - demoState->displayObjectAxes;
		break;

		// toggle tangent bases on vertices or other
	case 'B':
		demoState->displayTangentBases = 1 - demoState->displayTangentBases;
		break;


		// update animation
	case 'm':
		demoState->updateAnimation = 1 - demoState->updateAnimation;
		break;
	}


	// callback for current mode
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demoCB_keyCharPress_main(demoState, asciiKey,
			demoSubMode, demoOutput, demoSubModeCount, demoOutputCount);
		break;
	}*/
}

// ASCII key is held
A3DYLIBSYMBOL void a3demoCB_keyCharHold(a3_NetworkState* demoState, a3i32 asciiKey)
{
	a3keyboardSetStateASCII(demoState->demoState->keyboard, (a3byte)asciiKey);
	/*// persistent state update


	// callback for current mode
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demoCB_keyCharHold_main(demoState, asciiKey);
		break;
	}
	*/
}

// mouse button is clicked
A3DYLIBSYMBOL void a3demoCB_mouseClick(a3_NetworkState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->demoState->mouse, (a3_MouseButton)button, a3input_down);
	a3mouseSetPosition(demoState->demoState->mouse, cursorX, cursorY);
}

// mouse button is double-clicked
A3DYLIBSYMBOL void a3demoCB_mouseDoubleClick(a3_NetworkState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->demoState->mouse, (a3_MouseButton)button, a3input_down);
	a3mouseSetPosition(demoState->demoState->mouse, cursorX, cursorY);
}

// mouse button is released
A3DYLIBSYMBOL void a3demoCB_mouseRelease(a3_NetworkState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->demoState->mouse, (a3_MouseButton)button, a3input_up);
	a3mouseSetPosition(demoState->demoState->mouse, cursorX, cursorY);
}

// mouse wheel is turned
A3DYLIBSYMBOL void a3demoCB_mouseWheel(a3_NetworkState* demoState, a3i32 delta, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetStateWheel(demoState->demoState->mouse, (a3_MouseWheelState)delta);
	a3mouseSetPosition(demoState->demoState->mouse, cursorX, cursorY);
	// controlled camera when zooming
	/*a3_DemoCamera *camera;


	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		// can use this to change zoom
		// zoom should be faster farther away
		camera = demoState->camera + demoState->activeCamera;
		camera->fovy -= camera->ctrlZoomSpeed * (camera->fovy / a3real_oneeighty) * (a3f32)delta;
		camera->fovy = a3clamp(camera->ctrlZoomSpeed, a3real_oneeighty - camera->ctrlZoomSpeed, camera->fovy);
		a3demo_updateCameraProjection(camera);
		break;
	}*/
}

// mouse moves
A3DYLIBSYMBOL void a3demoCB_mouseMove(a3_NetworkState* demoState, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetPosition(demoState->demoState->mouse, cursorX, cursorY);
}

// mouse leaves window
A3DYLIBSYMBOL void a3demoCB_mouseLeave(a3_NetworkState* demoState)
{
	// reset mouse state or any buttons pressed will freeze
	a3mouseReset(demoState->demoState->mouse);
}


//-----------------------------------------------------------------------------
