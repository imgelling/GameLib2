#if !defined(GAMEERRORS_H)
#define GAMEERRORS_H

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
		GameDirectX9Specific,
		GameDirectX10Specific,
		GameDirectX11Specific,
		GameDirectX12Specific,
		GameContent
	};
	
	struct GameError
	{
		GameErrors lastErrorType = GameErrors::None;
		std::string lastErrorString = "None";
		explicit operator bool() const;
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
			case (uint32_t)GameErrors::GameDirectX9Specific: _stream << "GameDirectX9Specific"; break;
			case (uint32_t)GameErrors::GameDirectX10Specific: _stream << "GameDirectX10Specific"; break;
			case (uint32_t)GameErrors::GameDirectX11Specific: _stream << "GameDirectX11Specific"; break;
			case (uint32_t)GameErrors::GameDirectX12Specific: _stream << "GameDirectX12Specific"; break;
			case (uint32_t)GameErrors::GameContent: _stream << "GameContent"; break;
			default: _stream << "GameErrorUnknown"; break;
			}
			// Append the error text to general error code
			return _stream << " : " << error.lastErrorString << "\n";
		}
	};

	inline GameError::operator bool() const
	{
		return  (lastErrorType != GameErrors::None);
	}

}

#endif