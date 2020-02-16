#pragma once
#include <queue>

class Event;
class EventManager
{
public:
	EventManager();
	~EventManager();

	void HandleEvents();
	void AddEvent(Event* e1);
private:
	std::queue<Event*> events;
};