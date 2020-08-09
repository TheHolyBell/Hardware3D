#pragma once

#include <list>
#include <functional>

template<typename T>
class Event;

template<typename RetType, typename ...Args>
class Event<RetType(Args...)>
{
	using event_type = std::function<RetType(Args...)>;
	std::list<event_type> m_EventList;

public:
	Event& operator+=(event_type event)
	{
		m_EventList.push_back(event);
		return *this;
	}

	void Clear()
	{
		m_EventList.clear();
	}

	RetType Invoke(Args... args)
	{
		if (!m_EventList.empty())
			for (const auto& i : m_EventList)
				i(args...);
	}

	RetType operator()(Args... args)
	{
		Invoke(args...);
	}

	bool operator!=(std::nullptr_t)
	{
		return !m_EventList.empty();
	}

	bool operator==(std::nullptr_t)
	{
		return m_EventList.empty();
	}
};