#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
    return m_KeyStates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
	if (m_KeyBuffer.size() > 0u)
	{
		Keyboard::Event e = m_KeyBuffer.front();
		m_KeyBuffer.pop();
		return e;
	}

	return {};
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return m_KeyBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	m_KeyBuffer = std::queue<Event>();
}

std::optional<char> Keyboard::ReadChar() noexcept
{
	if (m_CharBuffer.size() > 0u)
	{
		unsigned char charcode = m_CharBuffer.front();
		m_CharBuffer.pop();
		return charcode;
	}
	return { };
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return m_CharBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
	m_CharBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	m_bAutorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	m_bAutorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return m_bAutorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	m_KeyStates[keycode] = true;
	m_KeyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(m_KeyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	m_KeyStates = false;
	m_KeyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(m_KeyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	m_CharBuffer.push(character);
	TrimBuffer(m_CharBuffer);
}

void Keyboard::ClearState() noexcept
{
	m_KeyStates.reset();
}
