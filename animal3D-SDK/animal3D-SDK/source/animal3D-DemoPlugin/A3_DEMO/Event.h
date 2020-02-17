#pragma once

#include <string>
class GameObject;
struct a3_NetworkingManager;


class Event
{
public:
	Event(){}
	~Event(){}

	bool isOriginal = true;

	virtual void Execute(a3_NetworkingManager* net) = 0;
};

class MoveEvent : public Event
{
public:
	MoveEvent(int x, int y, GameObject* target, bool);
	~MoveEvent();

	void Execute(a3_NetworkingManager* net);

private:
	GameObject* owner;
	int xPos = 0;
	int yPos = 0;
};

class StringEvent : public Event
{
public:
	StringEvent(std::string newString, GameObject* target, bool);
	~StringEvent();

	void Execute(a3_NetworkingManager* net);

private:
	GameObject* owner;
	std::string myString;
};

class BoolEvent : public Event
{
public:
	BoolEvent(bool newIsRed, GameObject* target, bool);
	~BoolEvent();

	void Execute(a3_NetworkingManager* net);

private:
	GameObject* owner;
	bool isRed;
}; 