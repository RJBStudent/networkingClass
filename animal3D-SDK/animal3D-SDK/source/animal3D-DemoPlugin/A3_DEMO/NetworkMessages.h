#pragma once

enum a3_NetGameMessages
{
	ID_CUSTOM_MESSAGE_START = ID_USER_PACKET_ENUM + 1,

	ID_GAME_MESSAGE_1,
	ID_MOVE_MESSAGE

};

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
