#ifndef __ANIMAL3D_CHATMANAGER_H
#define __ANIMAL3D_CHATMANAGER_H

#include "animal3D/animal3D.h"
//#include <vector>

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ChatManager				a3_ChatManager;
#endif	// __cplusplus


const unsigned int TEXT_ARRAY_SIZE = 500;


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


struct a3_ChatManager
{
	//char textInput[TEXT_ARRAY_SIZE];
	unsigned int inputIndex;	
	//std::vector<a3_Message> MessageList;
};


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // !A3_CHATMANAGER_H
