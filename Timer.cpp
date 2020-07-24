#include "Timer.h"

Timer::Timer()
	: m_Last(std::chrono::steady_clock::now())
{
}

float Timer::Mark()
{
	const auto _old = std::move(m_Last);
	m_Last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> _frameTime = m_Last - _old;
	return _frameTime.count();
}

float Timer::Peek() const
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_Last).count();
}
