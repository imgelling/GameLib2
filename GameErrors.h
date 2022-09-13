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
		GameRenderer,
		GameWindowsSpecific,
		GameOpenGLSpecific,
		GameVulkanSpecific,
		GameContent
	};
	
	struct GameError
	{
		GameErrors lastErrorType = GameErrors::None;
		std::string lastErrorString = "None";
		explicit operator bool() const;
		//friend std::ostream& operator<< (std::ostream& stream, const game::GameError error);
		friend std::ostream& operator<< (std::ostream& _stream, const GameError error)
		{
			uint32_t errID = (uint32_t)error.lastErrorType;
			switch (errID)
			{
			case (uint32_t)GameErrors::None: _stream << "GameErrorNone"; break;
			case (uint32_t)GameErrors::GameMemoryAllocation: _stream << "GameErrorMemoryAllocation"; break;
			case (uint32_t)GameErrors::GameInvalidParameter: _stream << "GameErrorInvalidParameters"; break;
			case (uint32_t)GameErrors::GameWindowsSpecific: _stream << "GameErrorWindowsSpecific"; break;
			case (uint32_t)GameErrors::GameOpenGLSpecific: _stream << "GameErrorOpenGLSpecific"; break;
			case (uint32_t)GameErrors::GameVulkanSpecific: _stream << "GameErrorVulkanSpecific"; break;
			case (uint32_t)GameErrors::GameRenderer: _stream << "GameRenderer"; break;
			case (uint32_t)GameErrors::GameContent: _stream << "GameContent"; break;
			default: return _stream << "GameErrorUnknown";
			}
			// Append the error text to general error code
			return _stream << " : " << error.lastErrorString << "\n";
		}
	};

	// Cant seems to get this one in the struct/class


}