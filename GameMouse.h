#pragma once
#include "GameMath.h"

#define geM_LEFT   0
#define geM_MIDDLE 1
#define geM_RIGHT  2
#define geM_X1 geMOUSE_RIGHT + 1
#define geM_X2 geMOUSE_X1 + 1

namespace game
{
	class Mouse
	{
	public:
		Mouse();
		~Mouse();
		bool IsButtonHeld(const int32_t button) noexcept;
		bool WasButtonPressed(const int32_t button) noexcept;
		bool WasButtonReleased(const int32_t button) noexcept;
		void HandleMouseMove(const int32_t xPosition, const int32_t yPosition) noexcept;
		void HandleMouseWheel(const int32_t delta) noexcept;
		void SetMouseState(const uint32_t button, const bool pressed);
		void ResetMouseValues() noexcept;
		Pointi GetPosition() const noexcept;
		Pointi GetPositionRelative() const noexcept;
		int32_t GetWheelDelta() const noexcept;
		void ShowMouse(const bool isShown);
		void UseMouseAcceleration(const bool useAcceleration) noexcept;
	private:
		int32_t _wheelDelta;
		Pointi _position;
		Pointi _positionOld;
		Pointi _positionRelative;
		bool* _currentButtonState;
		bool* _oldButtonState;
		int32_t _userMouseParams[3];
	};

	inline Mouse::Mouse()
	{
		// Save the user mouse parameters
		SystemParametersInfo(SPI_GETMOUSE, 0, _userMouseParams, 0);

		_wheelDelta = 0;
		_currentButtonState = new bool[10];
		_oldButtonState = new bool[10];
		for (uint8_t button = 0; button < 10; button++)
		{
			_currentButtonState[button] = false;
			_oldButtonState[button] = false;
		}
	}

	inline Mouse::~Mouse()
	{
		delete[] _currentButtonState;
		delete[] _oldButtonState;

		// Restore user mouse parameters
		SystemParametersInfo(SPI_SETMOUSE, 0, _userMouseParams, SPIF_SENDCHANGE);
	}

	inline void Mouse::UseMouseAcceleration(const bool useAcceleration) noexcept
	{
		int32_t mouseParams[3] = {};

		// Get the current values.
		SystemParametersInfo(SPI_GETMOUSE, 0, mouseParams, 0);

		// Modify the acceleration value as directed.
		mouseParams[2] = useAcceleration;

		// Update the system setting.
		SystemParametersInfo(SPI_SETMOUSE, 0, mouseParams, SPIF_SENDCHANGE);
	}

	inline bool Mouse::IsButtonHeld(const int32_t button) noexcept
	{
		if ((button < 0) || (button > 9)) return false;
		bool currentState = _currentButtonState[button];
		bool oldState = _oldButtonState[button];

		//_oldButtonState[button] = _currentButtonState[button];

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

	inline bool Mouse::WasButtonPressed(const int32_t button) noexcept
	{
		if ((button < 0) || (button > 9)) return false;

		bool currentState = _currentButtonState[button];
		bool oldState = _oldButtonState[button];

		//_oldButtonState[button] = _currentButtonState[button];

		return (!oldState && currentState);
	}

	inline bool Mouse::WasButtonReleased(const int32_t button) noexcept
	{
		if ((button < 0) || (button > 9)) return false;

		bool currentState = _currentButtonState[button];
		bool oldState = _oldButtonState[button];

		//_oldButtonState[button] = _currentButtonState[button];

		return (oldState && !currentState);
	}

	inline void Mouse::HandleMouseMove(const int32_t xPosition, const int32_t yPosition) noexcept
	{
		_positionOld = _position;

		_position = { xPosition, yPosition };

		_positionRelative = _position - _positionOld;

	}
	
	inline void Mouse::HandleMouseWheel(const int32_t delta) noexcept
	{
		_wheelDelta = delta;
	}

	inline void Mouse::SetMouseState(const uint32_t button, const bool pressed)
	{
		_oldButtonState[button] = _currentButtonState[button];
		_currentButtonState[button] = pressed;
	}

	inline void Mouse::ResetMouseValues() noexcept
	{
		_wheelDelta = 0;
		_positionRelative = { 0, 0 };
	}

	inline Pointi Mouse::GetPosition() const noexcept
	{
		return _position;
	}

	inline Pointi Mouse::GetPositionRelative() const noexcept
	{
		return _positionRelative;
	}

	inline int32_t Mouse::GetWheelDelta() const noexcept
	{
		return _wheelDelta;
	}

	inline void Mouse::ShowMouse(const bool isShown)
	{
#if defined(_WIN32)
		ShowCursor(isShown);
#elif defined(__linux__)
		// show mouse
#endif
	}
}
