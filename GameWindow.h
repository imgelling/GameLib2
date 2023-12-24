#pragma once
#include <string>

#include "GameAttributes.h"
#include "GameErrors.h"
#include "GameHelpers.h"


namespace game
{
	extern GameError lastError;
	class Window
	{
	public:
		Window();
		bool CreateTheWindow();
		void SetAttributes(const Attributes attrib);
		void SetWindowTitle(const std::string title);
		void ToggleFullScreen();
		void DoMessagePump();
#if defined(_WIN32)
		HWND GetHandle();
#elif defined(__linux__)

#endif
	private:
		Attributes _attributes;

		// Windows only stuff
#if defined(_WIN32)
		RECT _windowPositionSave;
		HWND _windowHandle;
		MONITORINFO _monitorInfo = { 0 };
		static LRESULT CALLBACK _WindowEventProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
	};

	inline Window::Window()
	{
#if defined(_WIN32)
		_windowHandle = NULL;
		_windowPositionSave = { 0 };
#elif defined(__linux__)
#endif
	}

	inline bool Window::CreateTheWindow()
	{
#if defined(_WIN32)
		WNDCLASS wc{};
		DWORD dwExStyle = 0;
		DWORD dwStyle = 0;
		int32_t windowWidth = 0;
		int32_t windowHeight = 0;
		HMONITOR monitorHandle = 0;
		RECT rWndRect = { 0 };
		int32_t adjustedWidth = 0;
		int32_t adjustedHeight = 0;
		int32_t windowLeftPosition = 0;
		int32_t windowTopPosition = 0;


		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpfnWndProc = _WindowEventProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.lpszMenuName = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszClassName = Wide("GAME_ENGINE");
		RegisterClass(&wc);

		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		if (!_attributes.WindowVisible)
		{
			dwExStyle = 0;
			dwStyle = 0;
		}
		// Dimensions of the window
		windowWidth = _attributes.WindowWidth;
		windowHeight = _attributes.WindowHeight;

		// Get info about the monitor
		monitorHandle = MonitorFromWindow(_windowHandle, MONITOR_DEFAULTTONEAREST);
		_monitorInfo = { sizeof(_monitorInfo) };

		// Get info about the monitor
		if (!GetMonitorInfo(monitorHandle, &_monitorInfo))
		{
			lastError = { GameErrors::GameWindowsSpecific, "GetMonitorInfo failed." };
			return false;
		}

		// pretend we are making a window -- added for toggle fullscreen
		if (_attributes.WindowVisible)
		{
			rWndRect = { 0, 0, windowWidth, windowHeight };
			AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
			adjustedWidth = rWndRect.right - rWndRect.left;
			adjustedHeight = rWndRect.bottom - rWndRect.top;
			windowLeftPosition = 0;
			windowTopPosition = 0;
			_windowPositionSave.left = ((_monitorInfo.rcMonitor.right >> 1) - (adjustedWidth >> 1));
			_windowPositionSave.top = ((_monitorInfo.rcMonitor.bottom >> 1) - (adjustedHeight >> 1));
			_windowPositionSave.right = _windowPositionSave.left + adjustedWidth;
			_windowPositionSave.bottom = _windowPositionSave.top + adjustedHeight;
			// to save the information
		}

		// If the window is fullscreen, change the style of the window
		if (_attributes.WindowFullscreen)
		{
			dwExStyle = 0;
			dwStyle = WS_VISIBLE | WS_POPUP;

			// Save max size of monitor
			windowWidth = _monitorInfo.rcMonitor.right;
			windowHeight = _monitorInfo.rcMonitor.bottom;
		}


		// Adjust window size to account for title bar and edges
		rWndRect = { 0, 0, windowWidth, windowHeight };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		adjustedWidth = rWndRect.right - rWndRect.left;
		adjustedHeight = rWndRect.bottom - rWndRect.top;
		windowLeftPosition = 0;
		windowTopPosition = 0;

		// Center the window
		if (!_attributes.WindowFullscreen)
		{
			windowLeftPosition = ((_monitorInfo.rcMonitor.right >> 1) - (adjustedWidth >> 1));
			windowTopPosition = ((_monitorInfo.rcMonitor.bottom >> 1) - (adjustedHeight >> 1));
		}

		// Create the actual window
		_windowHandle = CreateWindowEx(dwExStyle, Wide("GAME_ENGINE"), Wide(""), dwStyle,
			windowLeftPosition, windowTopPosition, adjustedWidth, adjustedHeight, NULL, NULL, GetModuleHandle(nullptr), this);
		if (!_windowHandle)
		{
			lastError = { GameErrors::GameWindowsSpecific, "Windows Error Number : " + std::to_string(GetLastError()) };
			return false;
		}

		// Set the window title
		SetWindowTitle(_attributes.WindowTitle);

		return true;
#elif defined(__linux__)
		return false;
#endif
	}

	inline void Window::SetAttributes(const Attributes attrib)
	{
		_attributes = attrib;
	}

	inline void Window::SetWindowTitle(const std::string title)
	{
#if defined(_WIN32)
#ifdef UNICODE
		SetWindowText(_windowHandle, ConvertToWide(title).c_str());
#else
		SetWindowText(olc_hWnd, s.c_str());
#endif
#elif defined(__linux__)

#endif
	}

	inline void Window::DoMessagePump()
	{
#if defined(_WIN32)
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#elif defined(__linux__)

#endif
	}

	inline void Window::ToggleFullScreen()
	{
#if defined(_WIN32)
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		_attributes.WindowFullscreen = !_attributes.WindowFullscreen;


		// swapping to fullscreen
		if (_attributes.WindowFullscreen)
		{
			dwExStyle = 0;
			dwStyle = WS_VISIBLE | WS_POPUP;
			GetWindowRect(_windowHandle, &_windowPositionSave);

		}

		// Set window styles
		SetWindowLongPtr(_windowHandle, GWL_EXSTYLE, dwExStyle);
		SetWindowLongPtr(_windowHandle, GWL_STYLE, dwStyle);

		// Swapping to window
		if (!_attributes.WindowFullscreen)
		{

			int32_t width = _windowPositionSave.right - _windowPositionSave.left;
			int32_t height = _windowPositionSave.bottom - _windowPositionSave.top;
			// Move window to old position for window
			SetWindowPos(_windowHandle, 0, _windowPositionSave.left, _windowPositionSave.top, width, height, SWP_DRAWFRAME | SWP_FRAMECHANGED);
		}
		else // swapping to fullscreen
		{
			// Move window to corners for fullscreen
			SetWindowPos(_windowHandle, 0, 0, 0, _monitorInfo.rcMonitor.right, _monitorInfo.rcMonitor.bottom, SWP_DRAWFRAME | SWP_FRAMECHANGED);
		}
#elif defined(__linux__)

#endif


	}

#if defined(_WIN32)
	inline HWND Window::GetHandle()
	{
		return _windowHandle;
	}
#elif defined(__linux__)

#endif
}