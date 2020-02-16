#include "EventManager.h"
#include "Event.h"

EventManager::EventManager()
{

}

EventManager::~EventManager()
{

}

void EventManager::HandleEvents()
{
	while (!events.empty())
	{
		Event* e = events.front();
		e->Execute();
		events.pop();
	}
}

void EventManager::AddEvent(Event* e1)
{
	events.push(e1);
}