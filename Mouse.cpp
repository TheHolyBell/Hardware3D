#include "Mouse.h"
#include <Windows.h>

bool Mouse::IsEmpty() const noexcept
{
	return m_Buffer.empty();
}

void Mouse::Flush() noexcept
{
	m_Buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
	m_X = x;
	m_Y = y;

	m_Buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	m_bIsInWindow = false;
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	m_bIsInWindow = true;
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	m_bLeftIsPressed = true;
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	m_bLeftIsPressed = false;
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	m_bRightIsPressed = true;
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	m_bRightIsPressed = false;
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	m_Buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (m_Buffer.size() > BufferSize)
		m_Buffer.pop();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	m_WheelDeltaCarry += delta;
	// generate events for every 120 
	while (m_WheelDeltaCarry >= WHEEL_DELTA)
	{
		m_WheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (m_WheelDeltaCarry <= -WHEEL_DELTA)
	{
		m_WheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { m_X, m_Y };
}

int Mouse::GetPosX() const noexcept
{
	return m_X;
}

int Mouse::GetPosY() const noexcept
{
	return m_Y;
}

bool Mouse::IsInWindow() const noexcept
{
	return m_bIsInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return m_bLeftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return m_bRightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (m_Buffer.size() > 0)
	{
		Mouse::Event e = m_Buffer.front();
		m_Buffer.pop();
		return e;
	}

	return Mouse::Event();
}
