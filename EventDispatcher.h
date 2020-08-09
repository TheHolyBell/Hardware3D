#pragma once

#include "Event.h"

class EventDispatcher
{
public:
	static void RegisterOnResize(std::function<void(int, int)> callback);
	static void InvokeOnResize(int Width, int Height);


private:
	static Event<void(int, int)> s_OnResize;
};