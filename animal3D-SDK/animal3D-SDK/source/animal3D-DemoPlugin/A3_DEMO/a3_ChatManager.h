#pragma once

#include "A3_DEMO/a3_DemoState.h"
#include "animal3D/animal3D.h"
#include <vector>

const unsigned int TEXT_ARRAY_SIZE = 500;


struct a3_ChatManager
{
	char textInput[TEXT_ARRAY_SIZE];
	unsigned int inputIndex;	
	std::vector<a3_Message> MessageList;
};

/// <summary>
/// Add Input
/// </summary>
a3i32 InputChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState);


/// <summary>
/// Update time left on messages
/// </summary>
a3i32 UpdateChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState);

/// <summary>
/// Render all messages in chat
/// </summary>
a3i32 RenderChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState);

/// <summary>
/// Add message to list from networked list
/// </summary>
a3i32 AddMessage(a3_ChatManager* chatManager, a3_NetChatMessage newMessage);

//Container of messages
struct a3_Message
{
	char message[TEXT_ARRAY_SIZE];
	char user[TEXT_ARRAY_SIZE];
	a3f64 messageLength;
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