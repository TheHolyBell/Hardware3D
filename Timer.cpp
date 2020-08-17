#include "Timer.h"

Timer::Timer()
	: m_Last(std::chrono::steady_clock::now())
{
}

double Timer::Mark()
{
	const auto _old = std::move(m_Last);
	m_Last = std::chrono::steady_clock::now();
	const std::chrono::duration<double> _frameTime = m_Last - _old;
	return _frameTime.count();
}

double Timer::Peek() const
{
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - m_Last).count();
}
