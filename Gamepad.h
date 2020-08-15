#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <utility>

enum Button
{
	GAMEPAD_DPAD_UP = 0x0001,
	GAMEPAD_DPAD_DOWN = 0x0002,
	GAMEPAD_DPAD_LEFT = 0x0004,
	GAMEPAD_DPAD_RIGHT = 0x0008,
	GAMEPAD_START = 0x0010,
	GAMEPAD_BACK = 0x0020,
	GAMEPAD_LEFT_THUMB = 0x0040,
	GAMEPAD_RIGHT_THUMB = 0x0080,
	GAMEPAD_LEFT_SHOULDER = 0x0100,
	GAMEPAD_RIGHT_SHOULDER = 0x0200,
	GAMEPAD_A = 0x1000,
	GAMEPAD_B = 0x2000,
	GAMEPAD_X = 0x4000,
	GAMEPAD_Y = 0x8000
};

class Gamepad
{
public:
	static Gamepad& Get();

	bool IsPressed(Button btn) const;
	void UpdateState();

	float LeftTrigger() const;
	float RightTrigger() const;

	std::pair<float, float> LeftStick() const;
	std::pair<float, float> RightStick() const;

	bool StateChanged() const;
private:
	Gamepad();
	XINPUT_STATE m_State = {};
	int m_PlayerID;
	DWORD m_PrevDWPacketNumber;
	bool m_bStateChanged = false;
};