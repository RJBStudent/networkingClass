#pragma once


class Event
{
public:
	Event(){}
	~Event(){}

	virtual void Execute() = 0;
	virtual void Dispatch() = 0;

};