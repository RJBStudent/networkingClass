#include "Event.h"
#include "GameObject.h"

//*******************************	MOVEMENT STUFF *************************************************
MoveEvent::MoveEvent(int x, int y, GameObject* target)
{
	xPos = x;
	yPos = y;
	owner = target;
}

MoveEvent::~MoveEvent()
{

}

void MoveEvent::Execute()
{
	owner->setX(xPos);
	owner->setY(yPos);
}

//*******************************	STRING STUFF *************************************************
StringEvent::StringEvent(std::string newString, GameObject* target)
{
	myString = newString;
	owner = target;
}

StringEvent::~StringEvent()
{

}

void StringEvent::Execute()
{
	owner->setString(myString);
}

//*******************************	BOOL STUFF *************************************************
BoolEvent::BoolEvent(bool newIsRed, GameObject* target)
{
	isRed = newIsRed;
	owner = target;
}

BoolEvent::~BoolEvent()
{

}

void BoolEvent::Execute()
{
	owner->setRed(isRed);
}