#pragma once

#include <string>
#include <iostream>

namespace game
{
	enum class GameErrors
	{
		None = 0,
		GameMemoryAllocation,
		GameInvalidParameter,
		GameWindowsSpecific,
		GameOpenGLSpecific
	};
	
	struct GameError
	{
		GameErrors lastErrorType = GameErrors::None;
		std::string lastErrorString = "None";
		explicit operator bool() const;
		void Clear(void);
	};

	// Cant seems to get this one in the struct/class

	std::ostream& operator<< (std::ostream& stream, const game::GameError error);
}