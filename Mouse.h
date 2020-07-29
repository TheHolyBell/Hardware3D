#pragma once
#include <queue>
#include <optional>

class Mouse
{
	friend class Window;
public:
	struct RawDelta
	{
		int x, y;
	};
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
		};
	private:
		Type m_Type;
		bool m_bLeftIsPressed = false;
		bool m_bRightIsPressed = false;
		int m_X = 0;
		int m_Y = 0;
	public:
		Event() noexcept = default;
		Event(Type type, const Mouse& parent) noexcept
			: m_Type(type),
			m_bLeftIsPressed(parent.m_bLeftIsPressed),
			m_bRightIsPressed(parent.m_bRightIsPressed),
			m_X(parent.m_X),
			m_Y(parent.m_Y)
		{}

		Type GetType() const noexcept
		{
			return m_Type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return { m_X,m_Y };
		}
		int GetPosX() const noexcept
		{
			return m_X;
		}
		int GetPosY() const noexcept
		{
			return m_Y;
		}
		bool LeftIsPressed() const noexcept
		{
			return m_bLeftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return m_bRightIsPressed;
		}
	};

public:
	Mouse() = default;
	
	Mouse(const Mouse& rhs) = delete;
	Mouse& operator=(const Mouse& rhs) = delete;

	Mouse(Mouse&& rhs) = delete;
	Mouse& operator=(Mouse&& rhs) = delete;

	std::pair<int, int> GetPos() const noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	std::optional<Mouse::Event> Read() noexcept;
	bool IsEmpty() const noexcept;
	void Flush() noexcept;
	void EnableRaw() noexcept;
	void DisableRaw() noexcept;
	bool RawEnabled() const noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnRawDelta(int dx, int dy) noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;

private:
	static constexpr unsigned int BufferSize = 16;
	int m_X;
	int m_Y;
	bool m_bLeftIsPressed = false;
	bool m_bRightIsPressed = false;
	bool m_bIsInWindow = false;
	int m_WheelDeltaCarry = 0;
	std::queue<Event> m_Buffer;

	bool m_bRawEnabled = false;
	std::queue<RawDelta> m_RawDeltaBuffer;
};