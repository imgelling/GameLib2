#if !defined(GAME_H)
#define GAME_H

/*
Change Log:
	- November 23, 2023 : Released Game Engine to public on GitHub
	- November 29, 2023 : 
		Features:
			* Shaders are now hard coded in the code. Real small. External files removed.
		Misc:
			* DirectX10 uses ComPtrs now.
			* DirectX10 and 11 use ComPtrs for saving/restoring the states now.
		Bug Fixes:
			* F10 window resize.
	- December 3, 2023:
		Features:
			* No external image loader needed now.
		Bug Fixes:
			* OpenGL, with debug mode on, would spam about a bad texture on Nvidia cards.
	- December 9, 2023:
		Features:
			* DirectX 12 properly resizes on window resize.
		Misc:
			* flushGPU added and bool removed from _WaitForPreviousFrame
	- December 13, 2023:
		Features:
			* Keyboard now supports text input (like a console window)
				- All visual stuff must be handles by you.
				- REMOVED - Stores old text input in a buffer and allows navigating through it with the 
				  up and down arrows  - REMOVED December 18, 2023
				- Allows for cursor movement with left and right arrows.
				- Does not do insertion for text, only overwrite.  Maybe added
				  later, but my dev machine doesn not have an insert key.
	- December 17, 2023:
		Features:
			* Up to 4 XInput compatible controllers supported
				- Inputs from buttons are true/false and thumbsticks are -1.0 to 1.0 and
				  triggers are 0.0f to 1.0f
				- Battery level can be checked
				- Rumble can be set
				- Individual deadzones for the thumbsticks can be set (0-32767)
				- Threshold (for considering presses) for triggers can be set (0-255)
				- Connection, disconnection, and if a controller was never connected has detection
	- December 18, 2023:
		Features:
			* Keyboard text input can now set restrictions on input
				- GAME_TEXT_INPUT_ALL allows all (default)
				- GAME_TEXT_INPUT_DIGIT allows only numbers '1' key to '0' key, no symbols/letters/spaces/etc
				- GAME_TEXT_INPUT_ALPHA allows only letters 'a' key to 'z' key and 'A' key to 'Z' keys.
				  no symbols/numbers/spaces/etc
				- All modes allow for cursor movement and delete and backspace
		Misc:
			* Removed text buffer in keyboard text input mode.  This needs to be somewhere else to allow
			  for user to make multiple or define how they work.  The code is still there, just commented out

*/

#define GAME_FRAME_BUFFER_BIT (1)
#define GAME_DEPTH_STENCIL_BUFFER_BIT (1 << 1)

// If user has not chosen a renderer, default to OpenGL
// This is horrible looking, better way?
#if !defined(GAME_SUPPORT_DIRECTX12)
#if !defined(GAME_SUPPORT_DIRECTX11)
#if !defined(GAME_SUPPORT_DIRECTX10)
#if !defined(GAME_SUPPORT_DIRECTX9)
#if !defined(GAME_SUPPORT_OPENGL)
#if !defined(GAME_SUPPORT_VULKAN)
#if !defined(GAME_SUPPORT_ALL)
#define GAME_SUPPORT_OPENGL
#endif
#endif
#endif
#endif
#endif
#endif
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>




#if defined(GAME_SUPPORT_DIRECTX12) || defined(GAME_SUPPORT_ALL)
#define GAME_DIRECTX12 5
#endif

#if defined(GAME_SUPPORT_DIRECTX11) || defined(GAME_SUPPORT_ALL)
#define GAME_DIRECTX11 4
#endif

#if defined(GAME_SUPPORT_DIRECTX10) || defined(GAME_SUPPORT_ALL)
#define GAME_DIRECTX10 6
#endif

#if defined(GAME_SUPPORT_DIRECTX9) || defined(GAME_SUPPORT_ALL)
#define GAME_DIRECTX9 2
#endif

#include "GameWindowsKeys.h"

#endif // _WIN32

#if defined(GAME_SUPPORT_ALL)

#endif

#if defined(GAME_SUPPORT_OPENGL) || defined(GAME_SUPPORT_ALL)
#define GAME_OPENGL 1
#endif

#if defined(GAME_SUPPORT_VULKAN) || defined(GAME_SUPPORT_ALL)
#define GAME_VULKAN 3
#endif

#if defined(_WIN32)

#pragma region DirectX12
#if defined(GAME_DIRECTX12)
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#include "GameRendererDX12.h"
#endif
#pragma endregion

#pragma region DirectX11
#if defined(GAME_DIRECTX11)
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "GameRendererDX11.h"
#endif
#pragma endregion

#pragma region DirectX10
#if defined(GAME_DIRECTX10)
#include <d3d10.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "GameRendererDX10.h"
#endif
#pragma endregion

#pragma region DirectX9
#if defined(GAME_DIRECTX9)
#include <d3d9.h>
#include <d3dcompiler.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "GameRendererDX9.h"
#endif
#pragma endregion

#pragma region XInput
#if !defined(GAME_NO_GAMEPAD)
#pragma comment(lib,"Xinput.lib")
#include "GameGamePad.h"
#endif
#pragma endregion

#endif // _WIN32

#pragma region Vulkan
#if defined(GAME_VULKAN)
#if defined (_WIN32)
#include "vulkan/vulkan.hpp"
#pragma comment(lib, "vulkan-1.lib")
#elif define(__linux__)

#endif
#include "GameRendererVK.h"
#endif
#pragma endregion

#pragma region OpenGL
#if defined(GAME_OPENGL)
#include <GL/gl.h>
#if defined(_WIN32)
#pragma comment(lib, "opengl32.lib")
#elif defined(__linux__)

#endif
#include "GameRendererGL.h"
#pragma endregion
#endif

#pragma region Engine
#include "GameAttributes.h"
#include "GameColor.h"
#include "GameEngine.h"
#include "GameErrors.h"
#include "GameHelpers.h"
#include "GameImageLoader.h"
#include "GameKeyboard.h"
#include "GameLogger.h"
#include "GameMath.h"
#include "GameMouse.h"
#if defined(GAME_ENABLE_NETWORKING)
#include "GameNetwork.h"
#endif
#include "GamePerformanceTimer.h"
#include "GamePixelMode.h"
#include "GameRandom.h"
#include "GameRendererBase.h"
#include "GameShader.h"
#include "GameSpriteBatch.h"
#include "GameSpriteFont.h"
#include "GameSpriteSheet.h"
#include "GameSystemInfo.h"
#include "GameTerminal.h"
#include "GameTexture2D.h"
#include "GameTimer.h"
#include "GameWindow.h"
#pragma endregion

namespace game
{
	// Pointer to the engine created
	Engine* enginePointer = nullptr;
	// GameEngine global error tracking
	GameError lastError;
	// Information about computer and graphics card
	SystemInfo systemInfo;
}

#endif