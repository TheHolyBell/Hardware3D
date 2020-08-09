#include "EventDispatcher.h"

Event<void(int, int)> EventDispatcher::s_OnResize;

void EventDispatcher::RegisterOnResize(std::function<void(int, int)> callback)
{
	s_OnResize += callback;
}

void EventDispatcher::InvokeOnResize(int Width, int Height)
{
	if (s_OnResize != nullptr)
		s_OnResize(Width, Height);
}
