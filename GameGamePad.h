#if !defined(GAMECONTROLLER_H)
#define GAMECONTROLLER_H

#include <stdint.h>
#include <Xinput.h>
#include <iostream>

#include "GameMath.h"

constexpr auto GAME_GAME_PAD_1 = 0;
constexpr auto GAME_GAME_PAD_2 = 1;
constexpr auto GAME_GAME_PAD_3 = 2;
constexpr auto GAME_GAME_PAD_4 = 3;


namespace game
{

// Gamepad stuff
#define geG_A XINPUT_GAMEPAD_A
#define geG_B XINPUT_GAMEPAD_B
#define geG_X XINPUT_GAMEPAD_X
#define geG_Y XINPUT_GAMEPAD_Y
#define geG_L_BUMPER XINPUT_GAMEPAD_LEFT_SHOULDER
#define geG_R_BUMBER XINPUT_GAMEPAD_RIGHT_SHOULDER
#define geG_START XINPUT_GAMEPAD_START
#define geG_BACK XINPUT_GAMEPAD_BACK
#define geG_L_TRIGGER 69
#define geG_R_TRIGGER 420
#define geG_L_THUMBSTICK XINPUT_GAMEPAD_LEFT_THUMB
#define geG_R_THUMBSTICK XINPUT_GAMEPAD_RIGHT_THUMB
#define geG_DPAD_UP XINPUT_GAMEPAD_DPAD_UP
#define geG_DPAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define geG_DPAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN
#define geG_DPAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT

// Battery stuff
#define GAME_GAMEPAD_BATTERY_LOW BATTERY_LEVEL_LOW
#define GAME_GAMEPAD_BATTERY_MEDIUM BATTERY_LEVEL_MEDIUM
#define GAME_GAMEPAD_BATTERY_FULL BATTERY_LEVEL_FULL
#define GAME_GAMEPAD_BATTERY_EMPTY BATTERY_LEVEL_EMPTY
#define GAME_GAMEPAD_WIRED BATTERY_TYPE_WIRED

	class GamePad
	{
	public:
		bool wasButtonPressed(const uint32_t button, const uint32_t pad) const;
		bool wasButtonReleased(const uint32_t button, const uint32_t pad) const;
		bool isButtonHeld(const uint32_t button, const uint32_t pad) const;
		Vector2f PositionOf(const uint32_t analog, const uint32_t pad) const;

		uint32_t BatteryLevel(const uint32_t pad) const;
		int32_t Id(const uint32_t pad) const;
		void Connection(bool& isConnected, bool& wasConnected, const uint32_t pad) const;

		void SetRumble(const Vector2f amount, const uint32_t pad);
		void Update();
		// negative dead zone will set default, otherwise 0-32767 valid
		void SetDeadZone(const int32_t left, const int32_t right, const uint32_t pad);
		// negative values will set default, otherwise 0-255 valid
		void SetTriggerThreshold(const int32_t threshold, const uint32_t pad);
		 
	private:
		struct _PadState
		{
			XINPUT_STATE currentState;
			XINPUT_STATE oldState;

			bool isConnected;
			bool wasConnected;
			int32_t id;
			uint32_t batteryLevel;
			bool isWired;
			int32_t lThumbstickDeadZone;
			int32_t rThumbstickDeadZone;
			int32_t triggerThreshold;
			_PadState()
			{
				isConnected = false;
				wasConnected = false;
				batteryLevel = GAME_GAMEPAD_BATTERY_EMPTY;
				isWired = false;
				lThumbstickDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
				rThumbstickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
				triggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				id = -1;
				ZeroMemory(&currentState, sizeof(XINPUT_STATE));
				ZeroMemory(&oldState, sizeof(XINPUT_STATE));
			}
		};

		_PadState _padState[XUSER_MAX_COUNT];
	};

	inline uint32_t GamePad::BatteryLevel(const uint32_t pad) const
	{
		return _padState[pad].batteryLevel;
	}

	inline void GamePad::SetTriggerThreshold(const int32_t threshold, const uint32_t pad)
	{
		if (threshold < 0)
		{
			_padState[pad].triggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
		}
		else
		{
			_padState[pad].triggerThreshold = threshold;
		}
	}

	inline void GamePad::SetDeadZone(const int32_t left, const int32_t right, const uint32_t pad)
	{
		if (left < 0)
		{
			_padState[pad].lThumbstickDeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		}
		else
		{
			_padState[pad].lThumbstickDeadZone = left;
		}

		if (right < 0)
		{
			_padState[pad].rThumbstickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		}
		else
		{
			_padState[pad].rThumbstickDeadZone = right;
		}
	}

	inline void GamePad::Update()
	{
		XINPUT_STATE currentState;
		XINPUT_BATTERY_INFORMATION batteryInfo;

		for (uint32_t pad = 0; pad < XUSER_MAX_COUNT; pad++)
		{
			ZeroMemory(&currentState, sizeof(XINPUT_STATE));
			ZeroMemory(&batteryInfo, sizeof(XINPUT_BATTERY_INFORMATION));

			if (XInputGetState(pad, &currentState) == ERROR_SUCCESS)
			{
				// GamePad is connected at port "pad"
				// so save the state information
				_padState[pad].id = pad;
				_padState[pad].isConnected = true;
				_padState[pad].wasConnected = true;
				_padState[pad].oldState = _padState[pad].currentState;
				_padState[pad].currentState = currentState;

				// Save the battery information or
				// and if the gamepad is wired
				XInputGetBatteryInformation(pad, BATTERY_DEVTYPE_GAMEPAD, &batteryInfo);
				_padState[pad].batteryLevel = batteryInfo.BatteryLevel;
				if (batteryInfo.BatteryType == GAME_GAMEPAD_WIRED)
				{
					_padState[pad].isWired = true;
				}
			}
			else
			{
				// Controller is not connected
				// or disconnected, so set all 
				// saved info to defaults.
				_padState[pad].id = -1;
				ZeroMemory(&_padState[pad].currentState, sizeof(XINPUT_STATE));
				ZeroMemory(&_padState[pad].oldState, sizeof(XINPUT_STATE));
				_padState[pad].isConnected = false;
				_padState[pad].batteryLevel = GAME_GAMEPAD_BATTERY_EMPTY;
				_padState[pad].isWired = false;
			}		
		}
	}

	inline int32_t GamePad::Id(const uint32_t pad) const
	{
		return _padState[pad].id;
	}

	inline void GamePad::Connection(bool& isConnected, bool& wasConnected, const uint32_t pad) const
	{
		isConnected = _padState[pad].isConnected;
		wasConnected = _padState[pad].wasConnected;
	}

	inline bool GamePad::wasButtonPressed(const uint32_t button, const uint32_t pad) const
	{
		if (!_padState[pad].isConnected) return false;

		if (button == geG_L_TRIGGER) return false;
		if (button == geG_R_TRIGGER) return false;

		bool currentState = _padState[pad].currentState.Gamepad.wButtons & button;
		bool oldState = _padState[pad].oldState.Gamepad.wButtons & button;

		//_padState[pad].oldState.Gamepad.wButtons = _padState[pad].currentState.Gamepad.wButtons & ~button;

		return (!oldState && currentState);
	}

	inline bool GamePad::wasButtonReleased(const uint32_t button, const uint32_t pad) const
	{
		if (!_padState[pad].isConnected) return false;

		if (button == geG_L_TRIGGER) return false;
		if (button == geG_R_TRIGGER) return false;

		bool currentState = _padState[pad].currentState.Gamepad.wButtons & button;
		bool oldState = _padState[pad].oldState.Gamepad.wButtons & button;

		//_padState[pad].oldState.Gamepad.wButtons = _padState[pad].currentState.Gamepad.wButtons & ~button;

		return (oldState && !currentState);

	}

	inline bool GamePad::isButtonHeld(const uint32_t button, const uint32_t pad) const
	{
		if (!_padState[pad].isConnected) return false;

		if (button == geG_L_TRIGGER) return false;
		if (button == geG_R_TRIGGER) return false;

		bool currentState = _padState[pad].currentState.Gamepad.wButtons & button;
		bool oldState = _padState[pad].oldState.Gamepad.wButtons & button;

		//_padState[pad].oldState.Gamepad.wButtons = _padState[pad].currentState.Gamepad.wButtons & ~button;
		
		if (currentState && oldState)
		{
			// Current state and old state are true, so the button is held
			return true;
		}
		else if (!oldState && currentState)
		{
			// Still consider button held if this is the first time pressed
			return true;
		}
		return false;
	}

	inline Vector2f GamePad::PositionOf(const uint32_t analog, const uint32_t pad) const
	{
		Vector2f normalizedPosition(0, 0);

		if (!_padState[pad].isConnected) return normalizedPosition;

		switch (analog)
		{
		case geG_L_TRIGGER:
			if (_padState[pad].currentState.Gamepad.bLeftTrigger >= _padState[pad].triggerThreshold)
			{
				normalizedPosition.x = _padState[pad].currentState.Gamepad.bLeftTrigger / 255.0f;
				normalizedPosition.y = normalizedPosition.x;
			}
			break;
		case geG_R_TRIGGER:
			if (_padState[pad].currentState.Gamepad.bRightTrigger >= _padState[pad].triggerThreshold)
			{
				normalizedPosition.x = _padState[pad].currentState.Gamepad.bRightTrigger / 255.0f;
				normalizedPosition.y = normalizedPosition.x;
			}
			break;
		case geG_L_THUMBSTICK:
			if (_padState[pad].currentState.Gamepad.sThumbLX <= -_padState[pad].lThumbstickDeadZone)
			{
				normalizedPosition.x = _padState[pad].currentState.Gamepad.sThumbLX / 32768.0f;
			}
			else if(_padState[pad].currentState.Gamepad.sThumbLX >= _padState[pad].lThumbstickDeadZone)
			{
				normalizedPosition.x = _padState[pad].currentState.Gamepad.sThumbLX / 32767.0f;
			}
			if (_padState[pad].currentState.Gamepad.sThumbLY <= -_padState[pad].lThumbstickDeadZone)
			{
				normalizedPosition.y = _padState[pad].currentState.Gamepad.sThumbLY / 32768.0f;
			}
			else if (_padState[pad].currentState.Gamepad.sThumbLY >= _padState[pad].lThumbstickDeadZone)
			{
				normalizedPosition.y = _padState[pad].currentState.Gamepad.sThumbLY / 32767.0f;
			}
			break;
		case geG_R_THUMBSTICK:
			if (_padState[pad].currentState.Gamepad.sThumbRX <= -_padState[pad].rThumbstickDeadZone)
			{
				normalizedPosition.x = _padState[pad].currentState.Gamepad.sThumbRX / 32768.0f;
			}
			else if (_padState[pad].currentState.Gamepad.sThumbRX >= _padState[pad].rThumbstickDeadZone)
			{
				normalizedPosition.x = _padState[pad].currentState.Gamepad.sThumbRX / 32767.0f;
			}
			if (_padState[pad].currentState.Gamepad.sThumbRY <= -_padState[pad].rThumbstickDeadZone)
			{
				normalizedPosition.y = _padState[pad].currentState.Gamepad.sThumbRY / 32768.0f;
			}
			else if (_padState[pad].currentState.Gamepad.sThumbRY >= _padState[pad].rThumbstickDeadZone)
			{
				normalizedPosition.y = _padState[pad].currentState.Gamepad.sThumbRY / 32767.0f;
			}
			break;
		default:
			break;
		}

		return normalizedPosition;
	}

	inline void GamePad::SetRumble(const Vector2f amount, const uint32_t pad)
	{

	}
}


#endif