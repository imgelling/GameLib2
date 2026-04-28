#pragma once

#if defined(_DEBUG)
#define GAME_ASSERT(condition)                                      \
    do																\
	{                                                               \
        if (!(condition))                                           \
		{                                                           \
            __debugbreak();                                         \
        }                                                           \
    } while (false)
#else
#define GAME_ASSERT(condition)
#endif