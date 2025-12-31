#pragma once
#include "GameMath.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

#define geM_LEFT   0
#define geM_MIDDLE 1
#define geM_RIGHT  2
#define geM_X1 geMOUSE_RIGHT + 1
#define geM_X2 geMOUSE_X1 + 1

namespace game
{
	struct MouseButtonState
	{
		bool pressed = false;
		bool released = false;
		bool held = false;
	};
	struct MouseState
	{
		MouseButtonState buttonState[10];
		game::Pointi position;
		game::Pointi positionDelta; // may need to be cached between frames?
		int32_t wheelDelta = 0;
	};

	class Mouse
	{
	public:

		bool hasFocus = true;
		Mouse();
		~Mouse();
		bool IsButtonHeld(const int32_t button) const noexcept;
		bool WasButtonPressed(const int32_t button) const noexcept;
		bool WasButtonReleased(const int32_t button) const noexcept;
		void HandleMouseMove(const int32_t xPosition, const int32_t yPosition) noexcept;
		void HandleMouseWheel(const int32_t delta) noexcept;
		void SetMouseState(const uint32_t button, const bool pressed);
		void ResetMouseValues() noexcept;
		Pointi GetPosition() const noexcept;
		Pointi GetPositionRelative() const noexcept;
		int32_t GetWheelDelta() const noexcept;
		void ShowMouse(const bool isShown);
		void UseMouseAcceleration(const bool useAcceleration) noexcept;
		void SetFocus(const bool isFocused)
		{
			hasFocus = isFocused;
		}
		MouseState GetState() const
		{
			return _mouseState;
		}
		void SaveState()
		{
			// Save buttons
			for (int i = 0; i < 10; i++)
			{
				_mouseState.buttonState[i].pressed = false;
				_mouseState.buttonState[i].released = false;
				if (_newButtonState[i] != _oldButtonState[i])
				{
					if (_newButtonState[i])
					{
						_mouseState.buttonState[i].pressed = !_mouseState.buttonState[i].held;
						_mouseState.buttonState[i].held = true;
					}
					else
					{
						_mouseState.buttonState[i].released = true;
						_mouseState.buttonState[i].held = false;
					}
				}
				_oldButtonState[i] = _newButtonState[i];
			}

			// Save position
			_mouseState.position = _position;

			// Save relative position, how much it moved
			_mouseState.positionDelta = _positionRelative;

			// Save wheel delta
			_mouseState.wheelDelta = _wheelDelta;
		}
	private:
		int32_t _wheelDelta;
		Pointi _position;
		Pointi _positionOld;
		Pointi _positionRelative;
		bool _newButtonState[10];
		bool _oldButtonState[10];
		MouseState _mouseState;// buttonState[10];
		int32_t _userMouseParams[3];
	};

	inline Mouse::Mouse()
	{
		// Save the user mouse parameters
		SystemParametersInfo(SPI_GETMOUSE, 0, _userMouseParams, 0);

		_wheelDelta = 0;
		for (uint8_t button = 0; button < 10; button++)
		{
			_newButtonState[button] = false;
			_oldButtonState[button] = false;
		}
	}

	inline Mouse::~Mouse()
	{
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

	inline bool Mouse::IsButtonHeld(const int32_t button) const noexcept
	{
		if ((button < 0) || (button > 9)) return false;
		return _mouseState.buttonState[button].held;
	}

	inline bool Mouse::WasButtonPressed(const int32_t button) const noexcept
	{
		if ((button < 0) || (button > 9)) return false;
		return _mouseState.buttonState[button].pressed;
	}

	inline bool Mouse::WasButtonReleased(const int32_t button) const noexcept
	{
		if ((button < 0) || (button > 9)) return false;
		return _mouseState.buttonState[button].released;
	}

	inline void Mouse::HandleMouseMove(const int32_t xPosition, const int32_t yPosition) noexcept
	{
		hasFocus = true;
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
		_newButtonState[button] = pressed;
	}

	inline void Mouse::ResetMouseValues() noexcept
	{
		_wheelDelta = 0;
		_positionRelative = { 0, 0 };
	}

	inline Pointi Mouse::GetPosition() const noexcept
	{
		return _mouseState.position;// _position;
	}

	inline Pointi Mouse::GetPositionRelative() const noexcept
	{
		return _mouseState.positionDelta;// _positionRelative;
	}

	inline int32_t Mouse::GetWheelDelta() const noexcept
	{
		return _mouseState.wheelDelta;// _wheelDelta;
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
