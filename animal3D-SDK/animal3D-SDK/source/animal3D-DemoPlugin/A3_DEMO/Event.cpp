#include "Event.h"
#include "GameObject.h"

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakNetTypes.h"
#include "NetworkMessages.h"
#include "a3_NetworkingManager.h"


enum a3_NetGameMessages
{
	ID_CUSTOM_MESSAGE_START = ID_USER_PACKET_ENUM,

	ID_GAME_MESSAGE_1,
	ID_MOVE_MESSAGE

};


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
	message.x = xPos;
	message.y = yPos;
	
	RakNet::RakPeerInterface* peer = (RakNet::RakPeerInterface*)net->peer;
	peer->Send(reinterpret_cast<char*>(&message), sizeof(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, (RakNet::SystemAddress)net->ip, false);
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
}