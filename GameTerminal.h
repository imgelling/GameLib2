#pragma once
#include <iostream>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "GameKeyboard.h"

namespace game
{
	class Terminal
	{
	public:
		Terminal()
		{
			std::cout << resetAll;
#if _WIN32
			DWORD consoleMode = 0;
			DWORD err = 0;

			// Set output mode to handle virtual terminal sequences

			// Get the handle
			_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			err = GetLastError();
			if (err)
			{
				if (err != ERROR_IO_PENDING) std::cout << "GetStdHandle() error " << err;
			}

			// Get the current console mode
			GetConsoleMode(_consoleHandle, &_initialConsoleMode);
			err = GetLastError();
			if (err)
			{

				if (err != ERROR_IO_PENDING) std::cout << "GetConsoleMode() error " << err;
			}

			// Add the virtual terminal processing flag to the console mode
			// if it already isn't enabled.
			if (_initialConsoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
			{
				// It is already set, just copy it over
				consoleMode = _initialConsoleMode;
			}
			else
			{
				// Not set, so we add the flag
				consoleMode = _initialConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			}

			SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);


			// Set the new console mode
			SetConsoleMode(_consoleHandle, consoleMode);
			err = GetLastError();
			if (err)
			{
				if (err != ERROR_IO_PENDING) std::cout << "SetConsoleMode() error " << err;
			}
#endif
		}
		~Terminal()
		{
#if _WIN32
			// Try to reset the console back to initial state
			SetConsoleMode(_consoleHandle, _initialConsoleMode);
#endif
		}

		HANDLE GetHandle()
		{
			return _consoleHandle;
		}

		bool IsFocused()
		{
			return _isFocused;
		}

		void CheckInputEvents(game::Keyboard &keyboard)
		{
			INPUT_RECORD inBuf[32];
			DWORD events = 0;
			// TODO: save the input handle
			if (!GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE), &events))
			{
				std::cout << GetLastError();

			}
			if (events > 0)
				ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), inBuf, events, &events);

			if (_isFocused)
				keyboard.ResetTextInputTextChange();

			for (uint32_t i = 0; i < events; i++)
			{
				switch (inBuf[i].EventType)
				{
				case KEY_EVENT:
				{
					KEY_EVENT_RECORD ker = inBuf[i].Event.KeyEvent;
					
					if (ker.bKeyDown)
					{ 
						for (int i = 0; i < ker.wRepeatCount; i++)
						{
							keyboard.SetKeyState((uint8_t)ker.wVirtualKeyCode, false);
							keyboard.SetKeyState((uint8_t)ker.wVirtualKeyCode, true);
						}
						continue;
					}
					else
					{
						keyboard.SetKeyState((uint8_t)ker.wVirtualKeyCode, false);
					}
				}
				break;
				case MOUSE_EVENT: 
				{
					//MOUSE_EVENT_RECORD mer = inBuf[i].Event.MouseEvent;

					//std::wcout << L"Mouse at (" << mer.dwMousePosition.X
					//	<< L"," << mer.dwMousePosition.Y << L")\n";

					//if (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
					//	std::wcout << L"  Left button pressed\n";
					//if (mer.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
					//	std::wcout << L"  Right button pressed\n";

					//if (mer.dwEventFlags == DOUBLE_CLICK)
					//	std::wcout << L"  Double click detected\n";
					//else if (mer.dwEventFlags == MOUSE_MOVED)
					//	std::wcout << L"  Mouse moved\n";
					//else if (mer.dwEventFlags == MOUSE_WHEELED)
					//	std::wcout << L"  Mouse wheel scrolled\n";
				}
				break;
				case FOCUS_EVENT:
				{
					if (inBuf[i].Event.FocusEvent.bSetFocus)
					{
						_isFocused = true;
					}
					else
					{
						_isFocused = false;
					}
				}
				break;
				//WINDOW_BUFFER_SIZE_EVENT
				//VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
				//{
				//	printf("Resize event\n");
				//	printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
				//}
				//case MOUSE_EVENT:
				//{
				//	switch (inBuf[i].Event.MouseEvent.dwEventFlags)
				//	{
				//	case MOUSE_MOVED:
				//	{
				//		//SHORT m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
				//		//SHORT m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
				//		//std::cout << term.SetPosition(2, 2) << term.EraseLineFromCursor << inBuf[i].Event.MouseEvent.dwMousePosition.X << "," << inBuf[i].Event.MouseEvent.dwMousePosition.Y;

				//	}
				//	break;

				//	//case 0:
				//	//{
				//	//	for (int m = 0; m < 5; m++)
				//	//		m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

				//	//}
				//	//break;

				//	default:
				//		break;
				//	}
				//}
				//break;

				default:
					break;
					// We don't care just at the moment
				}
			}
		}


		// Screen management codes
#pragma region Management
		const std::string resetAll = "\033[0m";
#pragma endregion

		// Cursor movement codes
#pragma region Cursor	
		const std::string MoveToHome = "\033[H";
		const std::string SetPosition(const uint16_t, const uint16_t);
		void GetPosition(uint16_t&, uint16_t&);
		const std::string MoveUp(const uint16_t);
		const std::string MoveDown(const uint16_t);
		const std::string MoveRight(const uint16_t);
		const std::string MoveLeft(const uint16_t);
		const std::string MoveDownToBegining(const uint16_t); // needs better name
		const std::string MoveUpToBegining(const uint16_t);// needs better name
		const std::string MoveToColumn(const uint16_t);
		const std::string HideCursor = "\033[?25l";
		const std::string ShowCursor = "\033[?25h";
		const std::string SavePosition = "\033[s";
		const std::string RestorePosition = "\033[u";

		const std::string CursorDefault = "\033[0 q";
		const std::string CursorBlinkingBox = "\033[1 q";
		const std::string CursorSteadyBox = "\033[2 q";
		const std::string CursorBlinkUnderline = "\033[3 q";
		const std::string CursorSteadyUnderline = "\033[4 q";
		const std::string CursorBlinkingBar = "\033[5 q";
		const std::string CursorSteadyBar = "\033[6 q";

#pragma endregion	

		// Erase codes
#pragma region Erase
		const std::string EraseLineToCursor = "\033[1K";
		const std::string EraseLineFromCursor = "\033[0K";
		const std::string EraseLine = "\033[2K";
		const std::string EraseScreenFromCursor = "\033[0J";
		const std::string EraseScreenToCursor = "\033[1J";
		const std::string EraseScreen = "\033[2J";
		const std::string EraseScreenNoScroll = "\033[3J"; // may just clear scroll buffer
#pragma endregion

		// Text color changing codes
#pragma region Color
		const std::string DefaultColorFG = "\033[39m";
		const std::string DefaultColorBG = "\033[49m";
		const std::string ColorFG(const uint8_t);
		const std::string ColorBG(const uint8_t);
		const std::string Color256FG(const uint8_t);
		const std::string Color256BG(const uint8_t);
		const std::string Color24BitFG(const uint8_t, const uint8_t, const uint8_t);
		const std::string Color24BitBG(const uint8_t, const uint8_t, const uint8_t);
#pragma endregion	

		// Text style codes
#pragma region Style 
		// Check which ones work in windows 10 by default (font)
		const std::string Bold = "\033[1m";
		const std::string Dim = "\033[2m";
		const std::string Italic = "\033[3m";
		const std::string Underline = "\033[4m";
		const std::string Blink = "\033[5m";
		const std::string Invert = "\033[7m";
		const std::string Invisible = "\033[8m";
		const std::string Strikethrough = "\033[9m";
#pragma endregion

		// Text color codes
#pragma region ColorValues
		const uint8_t Black = 30;
		const uint8_t Red = 31;
		const uint8_t Green = 32;
		const uint8_t Yellow = 33;
		const uint8_t Blue = 34;
		const uint8_t Magenta = 35;
		const uint8_t Cyan = 36;
		const uint8_t White = 37;
#pragma endregion

	private:
#ifdef _WIN32
		// Holds the intial (at startup) mode of the console to reset with at shutdown
		DWORD _initialConsoleMode = 0;
		// Holds the Windows handle of the console
		HANDLE _consoleHandle = NULL;
		// Is the console focused or not
		bool _isFocused = true;
#endif
	};

	//columnandrowstart a 1,1 being top left on linux, windows is 0,0--Note
	inline const std::string Terminal::SetPosition(const uint16_t column, const uint16_t row)
	{
#if _WIN32
		return "\033[" + std::to_string(row) + ";" + std::to_string(column) + "H";
#elif __linux
		return "\033[" + std::to_string(row + 1) + ";" + std::to_string(column + 1) + "H";
#endif
	}

	inline void Terminal::GetPosition(uint16_t& column, uint16_t& row)
	{
#ifdef _WIN32
		// TODO: this could use the saved handle
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		GetConsoleScreenBufferInfo(h, &bufferInfo);
		column = (uint16_t)bufferInfo.dwCursorPosition.X;
		row = (uint16_t)bufferInfo.dwCursorPosition.Y;
#else
		column = 0;
		row = 0;
#endif
	}

	inline const std::string Terminal::MoveDownToBegining(const uint16_t row)
	{
		return "\033[" + std::to_string(row) + "E";
	}

	inline const std::string Terminal::MoveUpToBegining(const uint16_t row)
	{
		return "\033[" + std::to_string(row) + "F";
	}

	inline const std::string Terminal::MoveToColumn(const uint16_t col)
	{
		return "\033[" + std::to_string(col) + "G";
	}

	inline const std::string Terminal::MoveUp(const uint16_t number)
	{
		return "\033[" + std::to_string(number) + "A";
	}

	inline const std::string Terminal::MoveDown(const uint16_t number)
	{
		return "\033[" + std::to_string(number) + "B";
	}

	inline const std::string Terminal::MoveRight(const uint16_t number)
	{
		return "\033[" + std::to_string(number) + "C";
	}

	inline const std::string Terminal::MoveLeft(const uint16_t number)
	{
		return "\033[" + std::to_string(number) + "D";
	}

	inline const std::string Terminal::ColorFG(const uint8_t color)
	{
		return "\033[" + std::to_string(color) + "m";
	}

	inline const std::string Terminal::ColorBG(const uint8_t color)
	{
		return "\033[" + std::to_string(color + 10) + "m";
	}

	inline const std::string Terminal::Color256FG(const uint8_t color)
	{
		return "\033[38;5;" + std::to_string(color) + "m";
	}

	inline const std::string Terminal::Color256BG(const uint8_t color)
	{
		return "\033[48;5;" + std::to_string(color) + "m";
	}

	inline const std::string Terminal::Color24BitFG(const uint8_t r, const uint8_t g, const uint8_t b)
	{
		return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
	}

	inline const std::string Terminal::Color24BitBG(const uint8_t r, const uint8_t g, const uint8_t b)
	{
		return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
	}

}
