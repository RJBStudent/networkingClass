#pragma once


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
