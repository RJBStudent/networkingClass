#include "Event.h"
#include "GameObject.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakNetTypes.h"
#include "NetworkMessages.h"
#include "a3_NetworkingManager.h"



//*******************************	MOVEMENT STUFF *************************************************
MoveEvent::MoveEvent(int x, int y, GameObject* target, bool isOrig)
{
	xPos = x;
	yPos = y;
	owner = target;
	isOriginal = isOrig;
}

MoveEvent::~MoveEvent()
{

}

void MoveEvent::Execute(a3_NetworkingManager* net)
{
	owner->setX(xPos);
	owner->setY(yPos);

	if (isOriginal)
	{
	MoveMessage message;
	message.messageId = ID_MOVE_MESSAGE;
	printf("SENDING MESSAGE %i\n", message.messageId);
	message.x = xPos;
	message.y = yPos;
	
	RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
	RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
	peer->Send(reinterpret_cast<char*>(&message), sizeof(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address , false);
	}
}

//*******************************	STRING STUFF *************************************************
StringEvent::StringEvent(std::string newString, GameObject* target, bool isOrig)
{
	myString = newString;
	owner = target;
	isOriginal = isOrig;
}

StringEvent::~StringEvent()
{

}

void StringEvent::Execute(a3_NetworkingManager* net)
{
	owner->setString(myString);
	if (isOriginal)
	{
		StringMessage message;
		message.messageId = ID_STRING_MESSAGE;
		printf("SENDING MESSAGE %i\n", message.messageId);
		strcpy(message.newString, myString.c_str());

		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
		peer->Send(reinterpret_cast<char*>(&message), sizeof(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);
	}
}

//*******************************	BOOL STUFF *************************************************
BoolEvent::BoolEvent(bool newIsRed, GameObject* target, bool isOrig)
{
	isRed = newIsRed;
	owner = target;
	isOriginal = isOrig;
}

BoolEvent::~BoolEvent()
{

}

void BoolEvent::Execute(a3_NetworkingManager* net)
{
	owner->setRed(isRed);
	if (isOriginal)
	{
		RedMessage message;
		message.messageId = ID_STRING_MESSAGE;
		printf("SENDING MESSAGE %i\n", message.messageId);
		message.isRed = isRed;

		RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
		RakNet::SystemAddress* address = (RakNet::SystemAddress*)net->connectedAddress;
		peer->Send(reinterpret_cast<char*>(&message), sizeof(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *address, false);
	}
}