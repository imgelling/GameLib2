#if !defined(GAMEHELPERS_H)
#define GAMEHELPERS_H

#include <string>

namespace game
{
	// --- Helpers Start
	enum class RenderAPI
	{
		OpenGL = 0,		// OpenGL
		Vulkan,			// Vulkan
		DirectX9,		// DirectXs
		DirectX10,		
		DirectX11,
		DirectX12
	};

	inline std::wstring ConvertToWide(const std::string s)
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

	inline uint64_t GetCPUCycles() noexcept
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

	// Macro to state a literal string is a wide string
#if defined(UNICODE) || defined(_UNICODE)
#define Wide(s) L##s
#else
#define Wide(s) s
#endif
}

#endif