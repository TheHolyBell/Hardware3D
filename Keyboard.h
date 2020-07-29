#pragma once

#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release
		};
	private:
		Type m_Type;
		unsigned char m_Code = 0;
	public:
		Event() noexcept = default;
		Event(Type type, unsigned char code) noexcept
			: m_Type(type),
			m_Code(code){}
		bool IsPress() const noexcept
		{
			return m_Type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return m_Type == Type::Release;
		}
		unsigned char GetCode() const noexcept
		{
			return m_Code;
		}
	};
public:
	Keyboard() = default;

	Keyboard(const Keyboard& rhs) = delete;
	Keyboard& operator=(const Keyboard& rhs) = delete;
	
	Keyboard(Keyboard&& rhs) = delete;
	Keyboard& operator=(Keyboard&& rhs) = delete;

	// key event stuff
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	// char event stuff
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept
	{
		while (buffer.size() > BufferSize)
		{
			buffer.pop();
		}
	}
private:
	static constexpr unsigned int NumKeys = 256u;
	static constexpr unsigned int BufferSize = 16u;

	bool m_bAutorepeatEnabled = false;
	std::bitset<NumKeys> m_KeyStates;
	std::queue<Event> m_KeyBuffer;
	std::queue<char> m_CharBuffer;
};
