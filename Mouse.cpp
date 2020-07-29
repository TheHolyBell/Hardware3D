#include "Mouse.h"
#include <Windows.h>

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { m_X,m_Y };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	if (m_RawDeltaBuffer.empty())
	{
		return std::nullopt;
	}
	const RawDelta d = m_RawDeltaBuffer.front();
	m_RawDeltaBuffer.pop();
	return d;
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

std::optional<Mouse::Event> Mouse::Read() noexcept
{
	if (m_Buffer.size() > 0u)
	{
		Mouse::Event e = m_Buffer.front();
		m_Buffer.pop();
		return e;
	}
	return {};
}

bool Mouse::IsEmpty() const noexcept
{
	return m_Buffer.empty();
}

void Mouse::Flush() noexcept
{
	m_Buffer = std::queue<Event>();
}

void Mouse::EnableRaw() noexcept
{
	m_bRawEnabled = true;
}

void Mouse::DisableRaw() noexcept
{
	m_bRawEnabled = false;
}

bool Mouse::RawEnabled() const noexcept
{
	return m_bRawEnabled;
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	m_X = newx;
	m_Y = newy;

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

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
	m_RawDeltaBuffer.push({ dx,dy });
	TrimRawInputBuffer();
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
	{
		m_Buffer.pop();
	}
}

void Mouse::TrimRawInputBuffer() noexcept
{
	while (m_RawDeltaBuffer.size() > BufferSize)
	{
		m_RawDeltaBuffer.pop();
	}
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