#pragma once
#include "GameIOCP_ErrorOutput.h"
#include "GameIOCP_IOCPManager.h"
#include "GameIOCPNetwork.h"
#include "GameIOCPNetwork_NetworkManager.h"
#include "GameIOCPNetwork_Packet.h"
#include "GameIOCPNetwork_Multicast.h"
#include "GameIOCPNetwork_UPNPPortForward.h"
#include "GameIOCPTimer_TimerManager.h"
#include "GameIOCPFile_FileManager.h"

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

// TODO: Needs to go somewhere else game helpers?
static const std::string FormatDataSize(const uint64_t bytes)
{
    const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    uint32_t unitIndex = 0;
    double_t bytesLeft = (double_t)bytes;
    while (bytesLeft >= 1024 && unitIndex < 4)
    {
        bytesLeft /= 1024.0;
        ++unitIndex;
    }

    std::ostringstream formattedSize;
    formattedSize << std::fixed << std::setprecision(2) << bytesLeft << " " << units[unitIndex] << std::defaultfloat;
    return formattedSize.str();
}