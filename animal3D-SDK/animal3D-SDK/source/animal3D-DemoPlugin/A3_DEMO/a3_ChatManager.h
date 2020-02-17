#ifndef __ANIMAL3D_CHATMANAGER_H
#define __ANIMAL3D_CHATMANAGER_H

#include "animal3D/animal3D.h"

struct a3_NetworkingManager;
struct a3_DemoState;


#define TEXT_ARRAY_SIZE 500
#define MAX_MESSAGES_RECEIVED 50
	//Container of messages
struct a3_Message
{
	char message[TEXT_ARRAY_SIZE];
	char user[TEXT_ARRAY_SIZE];
	a3f64 messageLength;
	int render;
};

//Used For Networking Messages
#pragma pack(push, 1)
struct a3_NetChatMessage
{
	unsigned char typeID;
	char message[TEXT_ARRAY_SIZE];
	char user[TEXT_ARRAY_SIZE];
};
#pragma pack (pop)



struct a3_ChatManager
{

	a3_Message messageList[MAX_MESSAGES_RECEIVED];

	char user[TEXT_ARRAY_SIZE];
	char textInput[TEXT_ARRAY_SIZE];
	unsigned int inputIndex;	

	enum ChatStates
	{
		STARTUP = 0,
		USERNAME_INPUT = 1,
		IN_CHAT = 2
	}states;
};
///<summary>
	///Initialize Chat Manager
	///</summary>
a3i32 InitChatManager(a3_ChatManager* chatManager);
/// <summary>
/// Add Input
/// </summary>
a3i32 InputChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState);
/// <summary>
/// Update time left on messages
/// </summary>
a3i32 UpdateChatManager(a3_ChatManager* chatManager, a3_DemoState* demoState, a3_NetworkingManager* net);
/// <summary>
/// Render all messages in chat
/// </summary>
a3i32 RenderChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState, a3_NetworkingManager* net );
/// <summary>
/// Add message to list from networked list
/// </summary>
//a3i32 AddMessage(a3_ChatManager* chatManager, a3_NetChatMessage newMessage);

#endif // !A3_CHATMANAGER_H
