#pragma once
#include <queue>

struct a3_NetworkingManager;

class Event;
class EventManager
{
public:
	EventManager();
	~EventManager();

	void HandleEvents(a3_NetworkingManager* net);
	void AddEvent(Event* e1);
private:
	std::queue<Event*> events;
};