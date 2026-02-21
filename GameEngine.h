#if !defined(GAMEENGINE_H)
#define GAMEENGINE_H

#if defined(__linux__)
#include <unistd.h>
#endif
//#define WIN32_LEAN_AND_MEAN
//#include <windowsx.h>

#include "GameAttributes.h"
#include "GameHelpers.h"
#include "GameKeyboard.h"
#include "GameLogger.h"
#include "GameMouse.h"
#include "GameRendererBase.h"
#include "GameSystemInfo.h"
#include "GameTimer.h"

#pragma region Vulkan
#if defined(GAME_VULKAN)
#include "GameRendererVK.h"
#endif
#pragma endregion

#pragma region DirectX11
#if defined(GAME_DIRECTX11)
#include "GameRendererDX11.h"
#endif
#pragma endregion

#pragma region DirectX10
#if defined(GAME_DIRECTX10)
#include "GameRendererDX10.h"
#endif
#pragma endregion

#pragma region DirectX9
#if defined(GAME_DIRECTX9)
#include "GameRendererDX9.h"
#endif
#pragma endregion

#pragma region Opengl
#if defined(GAME_OPENGL)
#include "GameRendererGL.h"
#endif
#pragma endregion



#include "GameTexture2D.h"
#include "GameMath.h"

namespace game
{
	// Makes computer choose dedicated over integrated gpus
#ifdef GAME_USE_DEDICATED_GPU
#if defined(_WIN32)
	extern "C"
	{
		__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}
#endif
#endif

	class Engine;
	extern Engine* enginePointer;		// Pointer to game engine for Windows messaging system
	extern SystemInfo systemInfo;		// Info about the cpu and gpu of the system
	
	class Engine
	{
		friend class PixelMode;
		friend class SpriteBatch;
	public:
		Keyboard geKeyboard;			// Provides key state info and text input
		Mouse geMouse;					// Provides mouse state info
		Logger* geLogger;				// Provides a logging interface
		bool geIsRunning;				// Is the engine running?
		bool geIsMinimized;				// Is the game window minimized?
		bool geIsMaximized;				// Is the game window maximized?
		bool geIsFullScreen;			// Is the game window full screen (no border)?
#if defined(GAME_DIRECTX9)
		LPDIRECT3DDEVICE9 d3d9Device;
#endif
#if defined(GAME_DIRECTX10)
		Microsoft::WRL::ComPtr<ID3D10Device> d3d10Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> d3d10SwapChain;
		Microsoft::WRL::ComPtr<ID3D10RenderTargetView> d3d10RenderTargetView;
		Microsoft::WRL::ComPtr<ID3D10DepthStencilView> d3d10DepthStencilView;
#endif
#if defined(GAME_DIRECTX11)
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3d11RenderTarget;
		Microsoft::WRL::ComPtr<ID3D11Device> d3d11Device;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> d3d11DepthStencilView;
#endif
#if defined (GAME_DIRECTX12)
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device;
		Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue;
#endif

		// Engine setup

		Engine();
		~Engine();
		// Sets the attributes of the game window and what
		// rendering API the engine uses.
		void geSetAttributes(const Attributes &attrib);
		// Creates the renderer and gets the engine ready to 
		// start.
		bool geCreate();
		// Starts the game engine loop.
		void geStartEngine();
		// Flags the engine to stop the game loop.
		void geStopEngine();
		// User sets the logger used by the engine.
		// Mandatory for the engine to start.
		void geSetLogger(Logger* logger);

		// Frame and update timing 
		
		// Returns the number of updates per second
		// the engine is running at.
		uint32_t geGetUpdatesPerSecond() const noexcept;
		// Returns the number of frames per second
		// the engine is running at.
		uint32_t geGetFramesPerSecond() const noexcept;
		// Returns the MHZ the cpu is running at.
		// Note: This basically returns the MHZ of the core
		// currently running, normally just base speed.
		// Probably need to do correctly if possible. Used to work
		// when the same code was in main.cpp in the update method...?
		uint32_t geGetCPUFrequency() const noexcept;
		// Sets the MAX frames per second the engine will render
		void geSetFrameLock(const uint32_t limit) noexcept;
		// Sets the MAX updates per second the engine will update
		void geSetUpdateLock(const uint32_t limit) noexcept;
		// Sets both the MAX updates and frames per second the engine do
		void geSetGameLocks(const uint32_t fps, const uint32_t ups) noexcept;

		
		// Renderer specific

		// Clears the current render target and/or depthstencil to
		// specified color.
		// Also starts the frame for DX9 and DX12. This 
		// needs to be called for them.
		void geClear(const uint32_t bufferFlags, const Color color);  // probably need a value for depth/stencil
		bool geCreateTexture(Texture2D& texture);
		bool geLoadTexture(const std::string &fileName, Texture2D& texture);
		void geUnLoadTexture(Texture2D& texture);
		bool geLoadShader(const std::string vertex, const std::string fragment, Shader& shader);
		bool geLoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader);
		bool geLoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader);
		void geUnLoadShader(Shader& shader);
		bool geIsUsing(const uint32_t renderer) const;
#if defined (GAME_DIRECTX12)
		RendererDX12* geGetRenderer()
		{
			return static_cast<RendererDX12*>(_renderer);
		}
#endif

		// Window stuff	

		Vector2i geGetWindowSize() const noexcept;
		void geSetWindowTitle(const std::string title);
		void geToggleFullscreen();
		void HandleWindowResize(const uint32_t width, const uint32_t height);
		
		// Created by end user		

		virtual void Update(const float_t msElapsed) = 0;
		virtual void Render(const float_t msElapsed) = 0;
		virtual void Initialize() = 0;
		virtual void LoadContent() = 0;
		virtual void Shutdown() = 0;

		virtual void HandleWindowSizeChange() {}

		// Tools
		void geLogLastError();

	private:
		float_t _frameTime;
		float_t _updateTime;
		Attributes _attributes;
		RendererBase* _renderer;
		Window _window;
		Timer _updateTimer;
		Timer _frameLockTimer;
		Timer _cpuSpeedTimer;
		uint32_t _cpuFrequency;
		uint32_t _updatesPerSecond;
		uint32_t _framesPerSecond;
		void _GetAndLogCPUInfo();
		void _ProcessMessages() noexcept;
		void _Swap();
	};

	bool g_trackingMouse = false;
	void StartMouseLeaveTracking(HWND hwnd) 
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;   // Track when mouse leaves the window
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = 0;// HOVER_DEFAULT;

		if (TrackMouseEvent(&tme)) {
			g_trackingMouse = true;
		}
	}

	inline Engine::Engine()
	{
		geIsRunning = false;
		geLogger = nullptr;
		enginePointer = this;
		_renderer = nullptr;
		_frameTime = 0.0f;
		_updateTime = 0.0f;
		_updatesPerSecond = 0;
		_framesPerSecond = 0;
		_cpuFrequency = 0;
		geIsMinimized = false;
		geIsMaximized = false;
		geIsFullScreen = false;
#if defined(GAME_DIRECTX9)
		d3d9Device = nullptr;
#endif
#if defined(GAME_DIRECTX10)
#endif
#if defined(GAME_DIRECTX11)
#endif
	}

	inline Engine::~Engine()
	{
		if (_renderer)
		{
			_renderer->DestroyDevice();
		}
#if defined(GAME_DIRECTX9)
		if (d3d9Device)
		{
			d3d9Device->Release();
		}
#endif
#if defined(GAME_DIRECTX10)
		d3d10SwapChain.Reset();
		d3d10Device.Reset();
		d3d10RenderTargetView.Reset();
		d3d10DepthStencilView.Reset();
#endif
#if defined(GAME_DIRECTX11)
#endif
#if defined (GAME_DIRECTX12)
#endif
		if (_renderer)
		{
			//_renderer->DestroyDevice();
			delete _renderer;
			_renderer = nullptr;
		}

	}

	inline bool Engine::geIsUsing(const uint32_t renderer) const
	{
#if defined(GAME_OPENGL)
		if ((renderer == 1) && (_attributes.RenderingAPI == RenderAPI::OpenGL))
			return true;
#endif
#if defined(GAME_DIRECTX9)
		if ((renderer == 2) && (_attributes.RenderingAPI == RenderAPI::DirectX9))
			return true;
#endif
#if defined(GAME_VULKAN)
		if ((renderer == 3) && (_attributes.RenderingAPI == RenderAPI::Vulkan))
			return true;
#endif
#if defined(GAME_DIRECTX10)
		if ((renderer == 6) && (_attributes.RenderingAPI == RenderAPI::DirectX10))
			return true;
#endif
#if defined(GAME_DIRECTX11)
		if ((renderer == 4) && (_attributes.RenderingAPI == RenderAPI::DirectX11))
			return true;
#endif
#if defined(GAME_DIRECTX12)
		if ((renderer == 5) && (_attributes.RenderingAPI == RenderAPI::DirectX12))
			return true;
#endif
		return false;
	}

	inline void Engine::geStartEngine()
	{
		// Storage of time
		float_t msElapsed = 0.0f;
		// Tracks updates per second
		double_t upsTime = 0.0f;
		uint32_t updatesCounted = 0;
		// Tracks frames per second
		float_t fpsTime = 0.0f;
		uint32_t framesCounted = 0;
	

		uint64_t cyclesStart = GetCPUCycles();

		geIsRunning = true;

		// Reset the timers
		_frameLockTimer.Reset();
		_updateTimer.Reset();
		_cpuSpeedTimer.Reset();

		// Do the game loop
		do
		{
			//geMouse.SaveState();
			//geMouse.ResetMouseValues();
			//geKeyboard.SaveState();

			// Do window messages
			_ProcessMessages();
			if (!geIsRunning) break;

			if (geIsMinimized)
			{
				#if defined(_WIN32)
				Sleep((uint32_t)(_updateTime));
				#elif defined(__linux__)
				sleep((uint32_t)(_updateTime));
				#endif
			}

			// Update cpu frequency
			if (_cpuSpeedTimer.Elapsed() > 1000.0f)
			{
				_cpuSpeedTimer.Reset();
				_cpuFrequency = (uint32_t)((GetCPUCycles() - cyclesStart) / 1000000);
				cyclesStart = GetCPUCycles();
			}

			// Update to updatelock
			msElapsed = _updateTimer.Elapsed();
			if (msElapsed >= _updateTime)
			{
				geMouse.SaveState();
				geMouse.ResetMouseValues();
				geKeyboard.SaveState();
				_updateTimer.Reset();
				Update(msElapsed);
				upsTime += msElapsed;
				updatesCounted++;
				if (upsTime >= 1000.0f)
				{
					_updatesPerSecond = updatesCounted;
					updatesCounted = 0;
					upsTime = upsTime - 1000.0f;
				}
			}

			// If software frame lock is on, make sure we adhere to that
			// and keep track of FPS
			msElapsed = _frameLockTimer.Elapsed();
			if (msElapsed >= _frameTime)
			{
				_frameLockTimer.Reset();
				Render(msElapsed);
				//std::cout << 1.0f / (msElapsed) << "\n";
				fpsTime += msElapsed;
				framesCounted++;
				if (fpsTime >= 1000.0f)
				{
					_framesPerSecond = framesCounted;
					framesCounted = 0;
					fpsTime = fpsTime - 1000.0f;
				}

				// Swap the buffers
				_Swap();
			}

		} while (geIsRunning);

		// Clean up end user stuff
		Shutdown();
	}

	inline void Engine::geStopEngine()
	{
		geIsRunning = false;
	}

	inline void Engine::geSetLogger(Logger* logger)
	{
		geLogger = logger;
	}

	inline void Engine::_ProcessMessages() noexcept
	{
		_window.DoMessagePump();
	}

	inline void Engine::geSetAttributes(const Attributes& attrib)
	{
		_attributes = attrib;
		if (_attributes.FrameLock > 0)
		{
			_frameTime = 1000.0f / _attributes.FrameLock;
		}
		else
		{
			_frameTime = 0.0f;
		}
		if (_attributes.UpdateLock > 0)
		{
			_updateTime = 1000.0f / _attributes.UpdateLock;
		}
		else
		{
			_updateTime = 0.0f;
		}
	}

	inline void Engine::geSetFrameLock(const uint32_t limit) noexcept
	{
		_attributes.FrameLock = (float_t)limit;
		if (_attributes.FrameLock > 0)
		{
			_frameTime = 1000.0f / _attributes.FrameLock;
		}
		else
		{
			_frameTime = 0.0f;
		}
	}

	inline void Engine::geSetUpdateLock(const uint32_t limit) noexcept
	{
		_attributes.UpdateLock = (float_t)limit;
		if (_attributes.UpdateLock > 0)
		{
			_updateTime = 1000.0f / _attributes.UpdateLock;
		}
		else
		{
			_updateTime = 0.0f;
		}
	}

	inline void Engine::geSetGameLocks(const uint32_t fps, const uint32_t ups) noexcept
	{
		geSetFrameLock(fps);
		geSetUpdateLock(ups);
	}

	inline uint32_t Engine::geGetCPUFrequency() const noexcept
	{
		return _cpuFrequency;
	}

	inline uint32_t Engine::geGetUpdatesPerSecond() const noexcept
	{
		return _updatesPerSecond;
	}

	inline uint32_t Engine::geGetFramesPerSecond() const noexcept
	{
		return _framesPerSecond;
	}

	inline Vector2i Engine::geGetWindowSize() const noexcept
	{
		return { (int)_attributes.WindowWidth, (int)_attributes.WindowHeight};
	}

	inline void Engine::geClear(const uint32_t bufferFlags, const Color color)
	{
		if (bufferFlags && _renderer)
		{
			_renderer->Clear(bufferFlags, color);
		}
	}
	
	inline bool Engine::geCreateTexture(Texture2D& texture)
	{
		if (_renderer)
		{
			return _renderer->CreateTexture(texture);
		}
		return false;
	}

	inline bool Engine::geLoadTexture(const std::string &fileName, Texture2D &texture)
	{
		if (_renderer)
		{
			return _renderer->LoadTexture(fileName, texture);
		}
		return false;
	}

	inline void Engine::geUnLoadTexture(Texture2D& texture)
	{
		if (_renderer)
		{
			_renderer->UnLoadTexture(texture);
		}
	}

	inline bool Engine::geLoadShader(const std::string vertex, const std::string fragment, Shader& shader)
	{
		if (_renderer)
		{
			return _renderer->LoadShader(vertex, fragment, shader);
		}
		return false;
	}

	inline bool Engine::geLoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader)
	{
		if (_renderer)
		{
			return _renderer->LoadTextShader(shaderText, vertexEntryPoint, fragmentEntryPoint, shader);
		}
		return false;
	}


	inline bool Engine::geLoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader)
	{
		if (_renderer)
		{
			return _renderer->LoadShader(vertex, fragment, geometry, shader);
		}
		return false;
	}

	inline void Engine::geUnLoadShader(Shader& shader)
	{
		if (_renderer)
		{
			_renderer->UnLoadShader(shader);
		}
	}

	inline void Engine::geSetWindowTitle(const std::string title)
	{
		_window.SetWindowTitle(title);
		_attributes.WindowTitle = title;
	}

	inline void Engine::geToggleFullscreen()
	{
		_window.ToggleFullScreen();
		_attributes.WindowFullscreen = !_attributes.WindowFullscreen;
		#if defined (_WIN32)
		RECT size;
		GetClientRect(_window.GetHandle(), &size);
		#elif defined(__linux__)
		Recti size;  // get window size on linux
		#endif
		//HandleWindowResize(size.right, size.bottom);
		geIsFullScreen = !geIsFullScreen;
	}

	inline void Engine::_GetAndLogCPUInfo()
	{
		std::stringstream sStream;

		// Lambda to log easier
		auto LOG = [&](std::stringstream& stream)
		{
			geLogger->Write(stream.str());
			stream.str("");
		};

		systemInfo.GetCPUInfo();

		sStream << "CPU vendor : " << systemInfo.cpuInfo.processorVendor;
		LOG(sStream);

		sStream << "CPU name : " << systemInfo.cpuInfo.processorName;
		LOG(sStream);

		sStream << "CPU processor thread count : " << systemInfo.cpuInfo.processorCount;
		LOG(sStream);

		sStream << "RAM physical total : " << systemInfo.ramInfo.totalPhysicalMemory << "MBs";
		LOG(sStream);

		sStream << "RAM free physical total : " << systemInfo.ramInfo.totalPhysicalMemoryAvailable << "MBs";
		LOG(sStream);
		
	}

	inline bool Engine::geCreate()
	{
		// Let user choose how they want things
		Initialize();

		// Make sure a logger is attached
		if (geLogger == nullptr)
		{
			throw std::logic_error("Must set a logger with geSetLogger() before calling geCreate().");
			//return false;
		}

		geLogger->Header(_attributes.WindowTitle, _attributes.GameVersion);

		// Get and log the cpu info
		_GetAndLogCPUInfo();

		// Create the window
		_window.SetAttributes(_attributes);
		if (!_window.CreateTheWindow())
		{
			return false;
		}
		// Track mouse leaving
		StartMouseLeaveTracking(_window.GetHandle());
		g_trackingMouse = true;

		// Set the renderer
		if (_attributes.RenderingAPI == RenderAPI::OpenGL)
		{
#if defined(GAME_OPENGL)
			_renderer = new game::RendererGL();
#else
			lastError = { GameErrors::GameInvalidParameter, "Requested OpenGL without #defining GAME_SUPPORT_OPENGL or GAME_SUPPORT ALL." };
			return false;
#endif
		}
		else if (_attributes.RenderingAPI == RenderAPI::Vulkan)
		{
			//lastError = { GameErrors::GameInvalidParameter, "Not implemented." };
#if defined(GAME_VULKAN)
			_renderer = new game::RendererVK;
#else
			lastError = { GameErrors::GameInvalidParameter, "Requested Vulkan without #defining GAME_SUPPORT_VULKAN or GAME_SUPPORT ALL." };
			return false;
#endif
		}
		else if (_attributes.RenderingAPI == RenderAPI::DirectX9)
		{
#if defined(GAME_DIRECTX9)
			_renderer = new game::RendererDX9();
#else
			lastError = { GameErrors::GameInvalidParameter, "Requested DirectX9 without #defining GAME_SUPPORT_DIRECTX9 or GAME_SUPPORT ALL." };
			return false;
#endif
		}
		else if (_attributes.RenderingAPI == RenderAPI::DirectX10)
		{
#if defined(GAME_DIRECTX10)
			_renderer = new game::RendererDX10();
#else
			lastError = { GameErrors::GameInvalidParameter, "Requested DirectX10 without #defining GAME_SUPPORT_DIRECTX10 or GAME_SUPPORT ALL." };
			return false;
#endif
		}
		else if (_attributes.RenderingAPI == RenderAPI::DirectX11)
		{
#if defined(GAME_DIRECTX11)
			_renderer = new game::RendererDX11();
#else
			lastError = { GameErrors::GameInvalidParameter, "Requested DirectX11 without #defining GAME_SUPPORT_DIRECTX11 or GAME_SUPPORT ALL." };
			return false;
#endif
		}
		else if (_attributes.RenderingAPI == RenderAPI::DirectX12)
		{
#if defined(GAME_DIRECTX12)
			_renderer = new game::RendererDX12();
#else
			lastError = { GameErrors::GameInvalidParameter, "Requested DirectX12 without #defining GAME_SUPPORT_DIRECTX12 or GAME_SUPPORT ALL." };
			return false;
#endif
		}
		else
		{
			lastError = { GameErrors::GameInvalidParameter, "Unknown Rendering Api requested." };
			return false;
		}
		_renderer->SetAttributes(_attributes, geLogger);

		// Create rendering device
		if (!_renderer->CreateDevice(_window))
		{
			_renderer->DestroyDevice();
			return false;
		}

		_renderer->FillOutRendererInfo();

#if defined(GAME_DIRECTX9)
		if (geIsUsing(GAME_DIRECTX9))
		{
			if (_renderer)
			{
				dynamic_cast<RendererDX9*>(_renderer)->GetDevice(d3d9Device);
			}
		}
#endif
#if defined(GAME_DIRECTX10)
		if (geIsUsing(GAME_DIRECTX10))
		{
			if (_renderer)
			{
				dynamic_cast<RendererDX10*>(_renderer)->GetDevice(d3d10Device, d3d10SwapChain, d3d10RenderTargetView, d3d10DepthStencilView);
			}
		}
#endif
#if defined(GAME_DIRECTX11)
		if (geIsUsing(GAME_DIRECTX11))
		{
			if (_renderer)
			{
				dynamic_cast<RendererDX11*>(_renderer)->GetDevice(d3d11Device, d3d11DeviceContext, d3d11RenderTarget, d3d11DepthStencilView);
			}
		}
#endif

#if defined(GAME_DIRECTX12)
		if (geIsUsing(GAME_DIRECTX12))
		{
			if (_renderer)
			{
				dynamic_cast<RendererDX12*>(_renderer)->GetDevice(d3d12Device, commandList, commandQueue);
			}
		}
#endif

		// Load user content
		LoadContent();

		return true;
	}

	inline void Engine::_Swap()
	{
		if (_renderer)
		{
			_renderer->Swap();
		}
	}

	inline void Engine::HandleWindowResize(const uint32_t width, const uint32_t height)
	{
		_attributes.WindowWidth = width;
		_attributes.WindowHeight = height;
#if defined(GAME_OPENGL)
		if (geIsUsing(GAME_OPENGL))
		{
			if (_renderer)
			{
				_renderer->HandleWindowResize(width, height);
			}
		}
#endif
#if defined(GAME_DIRECTX9)
			if (geIsUsing(GAME_DIRECTX9))
			{
				if (_renderer)
				{
					_renderer->HandleWindowResize(width, height);
					dynamic_cast<RendererDX9*>(_renderer)->GetDevice(d3d9Device);
				}
			}
#endif
#if defined(GAME_DIRECTX10)
		if (geIsUsing(GAME_DIRECTX10))
		{
			if (_renderer)
			{
				d3d10Device.Reset();
				d3d10SwapChain.Reset();
				d3d10RenderTargetView.Reset();
				_renderer->HandleWindowResize(width, height);
				dynamic_cast<RendererDX10*>(_renderer)->GetDevice(d3d10Device, d3d10SwapChain, d3d10RenderTargetView, d3d10DepthStencilView);
			}
		}
#endif
#if defined(GAME_DIRECTX11)
		if (geIsUsing(GAME_DIRECTX11))
		{
			if (_renderer)
			{
				d3d11RenderTarget.Reset();
				d3d11DepthStencilView.Reset();
				_renderer->HandleWindowResize(width, height);
				dynamic_cast<RendererDX11*>(_renderer)->GetDevice(d3d11Device, d3d11DeviceContext, d3d11RenderTarget, d3d11DepthStencilView);
			}
		}
#endif
#if defined(GAME_DIRECTX12)
		if (geIsUsing(GAME_DIRECTX12))
		{
			if (_renderer)
			{
				// These resets may not need resetting as I don't think they change
				d3d12Device.Reset();
				commandList.Reset();
				commandQueue.Reset();
				_renderer->HandleWindowResize(width, height);
				dynamic_cast<RendererDX12*>(_renderer)->GetDevice(d3d12Device, commandList, commandQueue);
			}
		}
#endif
		HandleWindowSizeChange();
	}

	inline void Engine::geLogLastError()
	{
		if (geLogger)
		{
			geLogger->Error(lastError);
		}
	}

#if defined(_WIN32)

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
	inline LRESULT CALLBACK Window::_WindowEventProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
		{
			if (!g_trackingMouse) 
			{
				StartMouseLeaveTracking(hWnd);
			}
			enginePointer->geMouse.HandleMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		}
		case WM_MOUSEWHEEL:	enginePointer->geMouse.HandleMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); return 0;
		//case WM_ERASEBKGND: return 1;
		case WM_MOUSELEAVE:
		{
			g_trackingMouse = false;
			enginePointer->geMouse.hasFocus = false; 
			return 0;
		}
		//case WM_NCMOUSELEAVE:
		//{
		//	g_trackingMouse = true;
		//	//StartMouseLeaveTracking(hWnd);
		//	enginePointer->geMouse.hasFocus = true;
		//	return 0;
		//}
		//case WM_SETFOCUS:	
		//	enginePointer->geMouse.hasFocus = true; return 0;// ptrPGE->olc_UpdateKeyFocus(true);                                       return 0;
		//case WM_KILLFOCUS:	
		//	enginePointer->geMouse.hasFocus = false; return 0;//ptrPGE->olc_UpdateKeyFocus(false);                                      return 0;
		case WM_LBUTTONDOWN: SetCapture(hWnd); enginePointer->geMouse.hasFocus = true; enginePointer->geMouse.SetMouseState(geM_LEFT, true); return 0;
		case WM_LBUTTONUP:	 ReleaseCapture(); enginePointer->geMouse.SetMouseState(geM_LEFT, false); return 0;
		case WM_RBUTTONDOWN:SetCapture(hWnd); enginePointer->geMouse.hasFocus = true; enginePointer->geMouse.SetMouseState(geM_RIGHT, true); return 0;
		case WM_RBUTTONUP:	 ReleaseCapture(); enginePointer->geMouse.SetMouseState(geM_RIGHT, false); return 0;
		case WM_MBUTTONDOWN:SetCapture(hWnd); enginePointer->geMouse.hasFocus = true; enginePointer->geMouse.SetMouseState(geM_MIDDLE, true); return 0;
		case WM_MBUTTONUP:	 ReleaseCapture(); enginePointer->geMouse.SetMouseState(geM_MIDDLE, false); return 0;
		case WM_XBUTTONDOWN: SetCapture(hWnd); enginePointer->geMouse.hasFocus = true; enginePointer->geMouse.hasFocus = true; enginePointer->geMouse.SetMouseState(GET_XBUTTON_WPARAM(wParam) + geM_RIGHT, true); return true;
		case WM_XBUTTONUP: ReleaseCapture(); enginePointer->geMouse.SetMouseState(GET_XBUTTON_WPARAM(wParam) + geM_RIGHT, false); return true;
		//case WM_SIZING: break;
			//WMSZ_BOTTOM
			//	6
			//	Bottom edge
			//	WMSZ_BOTTOMLEFT
			//	7
			//	Bottom - left corner
			//	WMSZ_BOTTOMRIGHT
			//	8
			//	Bottom - right corner
			//	WMSZ_LEFT
			//	1
			//	Left edge
			//	WMSZ_RIGHT
			//	2
			//	Right edge
			//	WMSZ_TOP
			//	3
			//	Top edge
			//	WMSZ_TOPLEFT
			//	4
			//	Top - left corner
			//	WMSZ_TOPRIGHT
			//	5
			//	Top - right corner
			// return true if we handled it
			// break
		case WM_SIZE: 
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
				enginePointer->geIsMinimized = true;
				enginePointer->geIsMaximized = false;
				/*enginePointer->HandleWindowResize(lParam & 0xFFF, (lParam >> 16) & 0xFFFF); */
				//enginePointer->HandleWindowResize(LOWORD(lParam), HIWORD(lParam));
				break;
			case SIZE_MAXIMIZED:
				enginePointer->geIsMinimized = false;
				enginePointer->geIsMaximized = true;
				//enginePointer->HandleWindowResize(lParam & 0xFFF, (lParam >> 16) & 0xFFFF); 
				//enginePointer->HandleWindowResize(LOWORD(lParam), HIWORD(lParam));
				break;
			case SIZE_RESTORED:
				//if (enginePointer->geIsMaximized)
				//{
					//enginePointer->HandleWindowResize(LOWORD(lParam), HIWORD(lParam));
					enginePointer->geIsMaximized = false;
				//}
				//if (enginePointer->geIsFullScreen)
				//{
					enginePointer->geIsFullScreen = false;
				//}
				enginePointer->geIsMinimized = false;
				break;
			default:
				break;
			}
			// Tell the application the window changed size
			enginePointer->HandleWindowResize(lParam & 0xFFF, (lParam >> 16) & 0xFFFF);

			return 0;
		}
		//case WM_EXITSIZEMOVE: 
		//{
		//	enginePointer->HandleWindowResize(enginePointer->geGetWindowSize().x, enginePointer->geGetWindowSize().y);
		//	return 0;
		//}

		case WM_KEYDOWN:
		{
			int repeatCount = lParam & 0xFFFF;
			for (int i = 0; i < repeatCount; i++)
			{
				enginePointer->geKeyboard.SetKeyState((uint8_t)wParam, false);
				enginePointer->geKeyboard.SetKeyState((uint8_t)wParam, true);
			}
			return 0;
		}
		case WM_KEYUP: enginePointer->geKeyboard.SetKeyState((uint8_t)wParam, false); return 0;
			//case WM_SYSKEYDOWN: ptrPGE->olc_UpdateKeyState(mapKeys[wParam], true);						return 0;
			//case WM_SYSKEYUP:	ptrPGE->olc_UpdateKeyState(mapKeys[wParam], false);
		case WM_CLOSE:		if (enginePointer) enginePointer->geIsRunning = false; return 0;
		case WM_DESTROY:	PostQuitMessage(0); DestroyWindow(hWnd); return 0;
		// Stops F10 from bringing up the window menu
		case WM_SYSCOMMAND: if ((wParam & 0xFFF0) == SC_KEYMENU) return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
#undef GET_X_LPARAM
#undef GET_Y_LPARAM 
	#elif defined(__linux__)
	
	#endif
}

#endif