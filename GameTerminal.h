#pragma once
#include <iostream>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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


		// Screen management codes
#pragma region Management
		const std::string resetAll = "\033[0m";
#pragma endregion

		// Cursor movement codes
#pragma region Cursor	
		const std::string MoveToHome = "\033[H";
		const std::string SetPosition(const uint16_t, const uint16_t);
		const void GetPosition(uint16_t&, uint16_t&);
		const std::string MoveUp(const uint16_t);
		const std::string MoveDown(const uint16_t);
		const std::string MoveRight(const uint16_t);
		const std::string MoveLeft(const uint16_t);
		const std::string MoveDownToBegining(const uint16_t); // needs better name
		const std::string MoveUpToBegining(const uint16_t);// needs better name
		const std::string MoveToColumn(const uint16_t);
		const std::string HideCursor = "\033[?25l";
		const std::string ShowCursor = "\033[?25h";
		const std::string SavePosition = "\0337";
		const std::string RestorePosition = "\0338";

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
		const std::string EraseScreenToCursor = "\033[1J";
		const std::string EraseScreenFromCursor = "\033[0J";
		const std::string EraseScreen = "\033[2J";
		const std::string EraseScreenNoScroll = "\033[3J";
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

	inline const void Terminal::GetPosition(uint16_t& column, uint16_t& row)
	{
#ifdef _WIN32
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		GetConsoleScreenBufferInfo(h, &bufferInfo);
		column = bufferInfo.dwCursorPosition.X;
		row = bufferInfo.dwCursorPosition.Y;
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
