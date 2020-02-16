#ifndef __ANIMAL3D_CHATMANAGER_H
#define __ANIMAL3D_CHATMANAGER_H

#include "animal3D/animal3D.h"

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ChatManager				a3_ChatManager;
typedef struct a3_Message					a3_Message;
#endif	// __cplusplus

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


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // !A3_CHATMANAGER_H
