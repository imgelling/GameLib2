#pragma once
#include <stdint.h>
#include <string>

//#define GAME_BREAK_ON_ERROR
namespace game
{
	namespace IOCP
	{
		// Outputs a error with more detail about it
		int32_t ErrorOutput(const std::string& command, const uint32_t line, const bool ignoreIOPending = true);
	}
}