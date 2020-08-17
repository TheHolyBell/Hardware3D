#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	double Mark();
	double Peek() const;
private:
	std::chrono::steady_clock::time_point m_Last;
};