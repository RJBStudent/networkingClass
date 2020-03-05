#pragma once
#include "RakNet/MessageIdentifiers.h"

enum a3_NetGameMessages
{
	ID_CUSTOM_MESSAGE_START = ID_USER_PACKET_ENUM + 1,

	ID_GAME_MESSAGE_1,
	ID_STRING_MESSAGE,
	ID_MOVE_MESSAGE,
	ID_ISRED_MESSAGE,
	ID_CHAT_MESSAGE,
	ID_CONNECTED_MESSAGE,
	ID_SET_BOID_ID,
	ID_SET_BOID_POS,
	ID_TARGET_BOID_POS,
	ID_SET_BOID_VEL
	
};


#define TEXT_ARRAY_SIZE 500
#pragma pack(push, 1)
struct MoveMessage
{
	int messageId = 0;
	int x = 0;
	int y = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct StringMessage
{
	int messageId = 0;
	char newString[512];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RedMessage
{
	int messageId = 0;
	bool isRed;
};
#pragma pack(pop)

//Used For Networking Messages
#pragma pack(push, 1)
struct a3_NetChatMessage
{
	unsigned char typeID;
	char message[TEXT_ARRAY_SIZE];
	char user[TEXT_ARRAY_SIZE];
};
#pragma pack (pop)

#pragma pack(push, 1)
struct IntMessage
{
	int messageId = 0;
	int intValue;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct Vector2Message
{
	int messageId = 0;
	float xValue[30];
	float yValue[30];
	int idIndex[30];
};
#pragma pack(pop)