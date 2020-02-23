#include "EventManager.h"
#include "Event.h"
#include "a3_NetworkingManager.h"

EventManager::EventManager()
{

}

EventManager::~EventManager()
{
	while (!events.empty())
	{
		Event* e = events.front();
		delete e;
		events.pop();
	}
}

void EventManager::HandleEvents(a3_NetworkingManager* net)
{
	while (!events.empty())
	{
		Event* e = events.front();
		e->Execute(net);
		delete e;
		events.pop();
	}
}

void EventManager::AddEvent(Event* e1)
{
	events.push(e1);
}