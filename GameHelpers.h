#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace game
{
	enum class RenderAPI
	{
		OpenGL = 0,		// OpenGL
		Vulkan,			// Vulkan
		DirectX11,
		DirectX12
	};

#pragma warning(push)
#pragma warning(disable : 4505) 

	static inline std::string ConvertFromWide(const std::wstring& wstr) 
	{
		if (wstr.empty()) return std::string();

		int sizeNeeded = WideCharToMultiByte(CP_UTF8,0,	wstr.c_str(),static_cast<int>(wstr.size()),	nullptr,0,nullptr, nullptr);

		std::string result(sizeNeeded, '\0');

		int bytesWritten = WideCharToMultiByte(
			CP_UTF8,
			0,
			wstr.c_str(),
			static_cast<int>(wstr.size()),
			&result[0],
			sizeNeeded,
			nullptr, nullptr
		);
		if (bytesWritten <= 0) {
			throw std::runtime_error("WideCharToMultiByte failed during conversion.");
		}
		return result;
	}

	static inline std::wstring ConvertToWide(const std::string &s)
	{
#if defined(_WIN32)
		uint32_t count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
		wchar_t* _buffer = new wchar_t[count];
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, _buffer, count);
		std::wstring wideString(_buffer);
		delete[] _buffer;
		return wideString;
#endif
#if defined(__linux__)
	std::wstring wideString;//= s;
	return wideString;
#endif
	}

	static inline uint64_t GetCPUCycles() noexcept
	{
#if defined(_WIN32)
		return __rdtsc();
#elif defined(__linux__)
		uint32_t lo = 0;
		uint32_t hi = 0;
		asm  volatile("rdtsc" : "=a" (lo), "=d" (hi));
		uint64_t cyclesStart = (((uint64_t)lo) | (((uint64_t)hi) << 32));
		return cyclesStart;
#endif
	}

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
}
#pragma warning(pop)