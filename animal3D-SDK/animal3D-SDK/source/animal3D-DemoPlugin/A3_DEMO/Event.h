#pragma once

//#include <string>
//class GameObject;

class Event
{
public:
	Event(){}
	~Event(){}

	virtual void Execute() = 0;
};
/*
class MoveEvent : public Event
{
public:
	MoveEvent(int x, int y, GameObject* target);
	~MoveEvent();

	void Execute();

private:
	GameObject* owner;
	int xPos = 0;
	int yPos = 0;
};

class StringEvent : public Event
{
public:
	//StringEvent(std::string newString, GameObject* target);
	~StringEvent();

	void Execute();

private:
	GameObject* owner;
//	std::string myString;
};

class BoolEvent : public Event
{
public:
	BoolEvent(bool newIsRed, GameObject* target);
	~BoolEvent();

	void Execute();

private:
	GameObject* owner;
	bool isRed;
}; 
*/