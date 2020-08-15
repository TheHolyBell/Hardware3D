#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <utility>

enum class Button
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
	// @ Get instance of Gamepad class (singleton implementation)
	static Gamepad& Get();

	// @ Checks whether particular button is pressed
	bool IsPressed(Button btn) const;
	// @ Updates state of XInput
	void UpdateState();

	// @ Returns normalized value of left trigger's pressure
	float LeftTrigger() const;
	// @ Returns normalized value of right trigger's pressure
	float RightTrigger() const;

	// @ Sets percentage of vibration power for both motors
	void SetVibration(int leftMotor, int rightMotor);
	// @ Returns percentage of vibration power for both motors
	std::pair<int, int> GetVibration() const;

	// @ Returns normalized values of left stick's x- and y-axis tilt
	std::pair<float, float> LeftStick() const;
	// @ Returns normalized values of right stick's x- and y-axis tilt
	std::pair<float, float> RightStick() const;

	// @ Check whether state of Gamepad has been changed since last UpdateState()
	bool StateChanged() const;

	void SetSensitivity(int sensitivity);
	int GetSensitivity() const;
private:
	Gamepad();
	XINPUT_STATE m_State = {};
	int m_PlayerID;
	DWORD m_PrevDWPacketNumber;
	int m_LeftMotorVibration = 0;
	int m_RightMotorVibration = 0;
	int m_Sensitivity = 0;
	bool m_bStateChanged = false;
};