#if !defined(GAMEATTRIBUTES_H)
#define GAMEATTRIBUTES_H
#include <math.h>
#include "GameHelpers.h"

namespace game
{
	struct Attributes
	{
		std::string WindowTitle;	// Title of the window created
		uint32_t WindowWidth;		// Width of the window created
		uint32_t WindowHeight;		// Height of the window created
		bool WindowFullscreen;		// Is the window created fullscreen
		bool WindowBorderless;		// Is the window created borderless
		bool WindowVisible;			// Used to hide a window
		uint8_t ContextMajor;		// Major version number (OpenGL only for now, may use for dx10,11,12)
		uint8_t ContextMinor;		// Minor version number (OpenGL)
		uint8_t RedSize;			// Size, in bits of red component of color depth
		uint8_t BlueSize;			// Size, in bits of blue component of color depth
		uint8_t GreenSize;			// Size, in bits of green component of color depth
		uint8_t AlphaSize;			// Size, in bits of alpha component of color depth
		uint8_t StencilSize;		// Size of stencil, may be GL only
		uint8_t DoubleBuffer;		// double_t buffer (Maybe triple... unsure), may need to be a bool
		uint8_t DepthSize;			// Size, in bits of depth buffer
		uint8_t MultiSamples;		// Multisampling, 1 is none, >1 enables multisampling
		bool DebugMode;				// Enable debug mode with the renderer
		float_t FrameLock;			// A software lock on max frames per second
		float_t UpdateLock;			// A software lock on max updates per second
		RenderAPI RenderingAPI;		// Which rendering API are we using
		bool VsyncOn;				// Is vsync on
		std::string GameVersion;	// What version the game is
		bool GLBackwardsCompatible;	// Is OpenGL backwards compatible?
		Attributes();				// Loads some defaults intro structure

	};

	inline Attributes::Attributes()
	{
		WindowTitle = "GameEngine Game";
		WindowWidth = 1280;
		WindowHeight = 720;
		WindowFullscreen = false;
		WindowBorderless = false;
		WindowVisible = true;
		ContextMajor = 3;
		ContextMinor = 2;
		RedSize = 8;
		BlueSize = 8;
		GreenSize = 8;
		AlphaSize = 8;
		StencilSize = 8;
		DoubleBuffer = 1; // may need to be -1 as was set in GameLib1
		GLBackwardsCompatible = true;
		DepthSize = 24; // 32 bit for amd only, 24 for nvidia
		MultiSamples = 0;
		DebugMode = false;
		FrameLock = 0.0f;
		UpdateLock = 0.0f;
		RenderingAPI = RenderAPI::OpenGL; // Defaults to OpenGL
		VsyncOn = false;
		GameVersion = "0";
	}
}

#endif