#pragma once

#include "d3dx12.h"
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>

#include <dxgidebug.h>
//#pragma comment(lib, "DXGIDebug.lib")

#include "GameErrors.h"
#include "GameHelpers.h"
#include "GameImageLoader.h"
#include "GameRendererBase.h"
#include "GameShader.h"
#include "GameSystemInfo.h"
#include "GameTexture2D.h"


#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#endif

#define frameBufferCount  2

namespace game
{
	extern SystemInfo systemInfo;

	void AppendHR12(const HRESULT hr)
	{
		if (hr == D3D12_ERROR_ADAPTER_NOT_FOUND)
			lastError.lastErrorString += ": D3D12_ERROR_ADAPTER_NOT_FOUND";
		else if (hr == D3D12_ERROR_DRIVER_VERSION_MISMATCH)
			lastError.lastErrorString += ": D3D12_ERROR_DRIVER_VERSION_MISMATCH";
		else if (hr == DXGI_ERROR_INVALID_CALL)
			lastError.lastErrorString += ": DXGI_ERROR_INVALID_CALL";
		else if (hr == DXGI_ERROR_WAS_STILL_DRAWING)
			lastError.lastErrorString += ": DXGI_ERROR_WAS_STILL_DRAWING";
		else if (hr == E_FAIL)
			lastError.lastErrorString += ": E_FAIL";
		else if (hr == E_INVALIDARG)
			lastError.lastErrorString += ": E_INVALIDARG";
		else if (hr == E_OUTOFMEMORY)
			lastError.lastErrorString += ": E_OUTOFMEMORY";
		else if (hr == E_NOTIMPL)
			lastError.lastErrorString += ": E_NOTIMPL";
	}
	class RendererDX12 : public RendererBase
	{
	public:
		RendererDX12();
		bool CreateDevice(Window& window);
		void DestroyDevice();
		void Swap();
		void HandleWindowResize(const uint32_t width, const uint32_t height);

		void FillOutRendererInfo() {};
		bool CreateTexture(Texture2D& texture);
		bool LoadTexture(std::string fileName, Texture2D& texture);
		void UnLoadTexture(Texture2D& texture) {};
		bool LoadShader(const std::string vertex, const std::string fragment, Shader& shader);
		bool LoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader)
		{
			lastError = { GameErrors::GameDirectX12Specific, "Geometry shaders not implemented yet." };
			return false;
		}
		bool LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader);
		void UnLoadShader(Shader& shader);
		void GetDevice(Microsoft::WRL::ComPtr<ID3D12Device2> &d3d12Device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList, Microsoft::WRL::ComPtr <ID3D12CommandQueue> &commandQueue);
		void Clear(const uint32_t bufferFlags, const Color color);

		// Below can go away if implement reset and execute
		Microsoft::WRL::ComPtr<ID3D12Fence> _fence[frameBufferCount];    // an object that is locked while our command list is being executed by the gpu. We need as many 
		//as we have allocators (more if we want to know when the gpu is finished with an asset)
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _commandAllocator[frameBufferCount]; // we want enough allocators for each buffer * number of threads (we only have one thread)
		HANDLE _fenceEvent; // a handle to an event when our fence is unlocked by the gpu
		uint64_t _fenceValue[frameBufferCount]; // this value is incremented each frame. each fence will have its own value
		uint32_t _frameIndex; // current rtv we are on
		void flushGPU();
	protected:
		void _ReadExtensions() {};
	private:
		void _WaitForPreviousFrame();
		void _StartFrame(); 
		bool _windowResized;
		CD3DX12_CPU_DESCRIPTOR_HANDLE _currentFrameBuffer;
		bool _midFrame; // Are we in the middle of a frame? If so end the frame before closing (dx12 does not like that)
		int32_t _allowTearing;
		D3D12_VIEWPORT _viewPort = {}; // area that output from rasterizer will be stretched to.
		D3D12_RECT _scissorRect = {}; // the area to draw in. pixels outside that area will not be drawn onto
		void _DoWindowResize();

		Microsoft::WRL::ComPtr<ID3D12Debug> _debugInterface;
		Microsoft::WRL::ComPtr<ID3D12Device2> _d3d12Device; // direct3d device
		Microsoft::WRL::ComPtr <ID3D12CommandQueue> _commandQueue; // container for command lists
		Microsoft::WRL::ComPtr <IDXGISwapChain3> _swapChain; // swapchain used to switch between render targets
		Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> _rtvDescriptorHeap; // a descriptor heap to hold resources like the render targets
		uint32_t _rtvDescriptorSize; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
		Microsoft::WRL::ComPtr <ID3D12Resource> _renderTargets[frameBufferCount]; // number of render targets equal to buffer count
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList; // a command list we can record commands into, then execute them to render the frame

	};

	inline void RendererDX12::GetDevice(Microsoft::WRL::ComPtr<ID3D12Device2> &d3d12Device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList, Microsoft::WRL::ComPtr <ID3D12CommandQueue> &commandQueue)
	{
		d3d12Device = _d3d12Device;
		commandList = _commandList;
		commandQueue = _commandQueue;
	}

	inline RendererDX12::RendererDX12()
	{
		_d3d12Device = nullptr;
		_commandQueue = nullptr;
		_swapChain = nullptr;
		_rtvDescriptorHeap = nullptr;
		_frameIndex = 0;
		_rtvDescriptorSize = 0;
		for (uint32_t buffer = 0; buffer < frameBufferCount; buffer++)
		{
			_renderTargets[buffer] = nullptr;
			_commandAllocator[buffer] = nullptr;
			_fence[buffer] = nullptr;
			_fenceValue[buffer] = 0;
		}
		_commandList = nullptr;
		_fenceEvent = nullptr;
		_midFrame = false;
		_allowTearing = 0;
		_windowResized = false;
	}

	inline void RendererDX12::DestroyDevice()
	{
		// Not sure how we are getting here mid frame, but we are, so make sure we finish executing
		// command list
		if (_midFrame)
		{
			Swap();
		}

		// get swapchain out of full screen before exiting
		BOOL fs = false;
		if (_swapChain)
		{
			_swapChain->GetFullscreenState(&fs, NULL);
			if (fs)
				_swapChain->SetFullscreenState(false, NULL);

			// wait for the gpu to finish all frames
			flushGPU();
		}

		// memory check stuff
		//Microsoft::WRL::ComPtr<IDXGIDebug1> pDebug;// = nullptr;
		//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
		//{
		//	pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		//	//pDebug->Release();
		//}

		CloseHandle(_fenceEvent);
		_fenceEvent = nullptr;
	}

	inline void RendererDX12::HandleWindowResize(const uint32_t width, const uint32_t height)
	{
		// Save new size
		_attributes.WindowWidth = width;
		_attributes.WindowHeight = height;
		_windowResized = true;
		//_DoWindowResize();
	};

	inline void RendererDX12::flushGPU()
	{
		for (int i = 0; i < frameBufferCount; i++)
		{
			uint64_t fenceValueForSignal = ++ _fenceValue[i];
			_commandQueue->Signal(_fence[i].Get(), fenceValueForSignal);
			if (_fence[i]->GetCompletedValue() < _fenceValue[i])
			{
				_fence[i]->SetEventOnCompletion(fenceValueForSignal, _fenceEvent);
				WaitForSingleObject(_fenceEvent, INFINITE);
			}
		}
		_frameIndex = 0;
		// Thank you gamedev
	}

	inline void RendererDX12::_DoWindowResize()
	{
		_windowResized = false;

		flushGPU();

		// Release the previous resources we will be recreating.
		for (int i = 0; i < frameBufferCount; i++)
		{
			_renderTargets[i].Reset();
		}

		// Here we would reset the depthstencil... if we HAD one!
		//_DepthStencilBuffer.Reset();

		uint32_t flags = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		if (!_attributes.VsyncOn || _allowTearing)
		{
			flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}


		if (FAILED(_swapChain->ResizeBuffers(frameBufferCount, _attributes.WindowWidth, _attributes.WindowHeight,
			DXGI_FORMAT_UNKNOWN,
			flags)))
		{
			std::cout << "Resize buffers failed\n";
			throw;
		}	

		// Handle to start of RTV heap
		CD3DX12_CPU_DESCRIPTOR_HANDLE  rtvHandle(_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each buffer
		for (int i = 0; i < frameBufferCount; i++)
		{
			if (FAILED(_swapChain->GetBuffer(i, IID_PPV_ARGS(_renderTargets[i].GetAddressOf()))))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not get buffer for RTV " + std::to_string(i) };
				return;
			}
			_renderTargets[i]->SetName(L"FrameBuffer");

			// Create RTV
			_d3d12Device->CreateRenderTargetView(_renderTargets[i].Get(), nullptr, rtvHandle);

			// Increment the rtv handle by the rtv descriptor size
			rtvHandle.Offset(1, _rtvDescriptorSize);
		}

		// Fill out the Viewport
		_viewPort.TopLeftX = 0;
		_viewPort.TopLeftY = 0;
		_viewPort.Width = (float_t)_attributes.WindowWidth;
		_viewPort.Height = (float_t)_attributes.WindowHeight;
		_viewPort.MinDepth = 0.0f;
		_viewPort.MaxDepth = 1.0f;

		// Fill out a scissor rect
		_scissorRect.left = 0;
		_scissorRect.top = 0;
		_scissorRect.right = _attributes.WindowWidth;
		_scissorRect.bottom = _attributes.WindowHeight;

		// Set the command list to record, don't need a pipeline state
		_commandList->Reset(_commandAllocator[_frameIndex].Get(), NULL);

		_commandList->RSSetViewports(1, &_viewPort);
		_commandList->RSSetScissorRects(1, &_scissorRect);

		_commandList->Close();

		// EXECUTE
		{
			ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };

			// execute the array of command lists
			_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			// this command goes in at the end of our command queue. This actually sets the fence value
			_commandQueue->Signal(_fence[_frameIndex].Get(), _fenceValue[_frameIndex]);
		}
		flushGPU();
	}

	inline bool RendererDX12::CreateDevice(Window& window)
	{
		uint32_t createFactoryFlags = 0;
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapterList;  // needs in class
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter = nullptr;
		uint32_t adapterIndex = 0;
		
		
		if (_attributes.DebugMode)
		{
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
		}

		// Enable debug mode if needed
		if (_attributes.DebugMode)
		{
			if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&_debugInterface))))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not get debug layer." };
				return false;
			}
			_debugInterface->EnableDebugLayer();
		}



		Microsoft::WRL::ComPtr <IDXGIFactory4> dxgiFactory;
		if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not create DXGIFactory." };
			return false;
		}


		// Enumerate all adapters for dx12 support
		while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			// Ignore software adapters
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapterIndex++;
				continue;
			}
			std::cout << "description : ";
			std::wcout << desc.Description;
			std::cout << "\n";
			std::cout << "RAM : " << desc.DedicatedVideoMemory / 1024 / 1024 << "MB\n";
			std::cout << "sys RAM : " << desc.DedicatedSystemMemory / 1024 / 1024 << "MB\n";
			std::cout << "Vendor id : " << desc.VendorId << "\n";

			// Check for dx12 support on current adapter
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				// Save the adapter
				adapterList.emplace_back(adapter);
			}

			// Check the next adapter
			adapterIndex++;
		}

		// If list is empty, no suitable adapter found
		if (adapterList.empty())
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not find a DirectX 12 adapter." };
			return false;
		}

		// Create the device
		HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_d3d12Device));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not create device." };
			AppendHR12(hr);
			return false;
		}

		// Check for variable refresh rate
		Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(dxgiFactory.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &_allowTearing, sizeof(_allowTearing))))
			{
				std::cout << "Check Feature Support failed." << _allowTearing << "\n";
			}
			std::cout << "Allow Tearing = " << (_allowTearing ? "true" : "false") << "\n";
			// Disable alt enter
			factory5->MakeWindowAssociation(window.GetHandle(), DXGI_MWA_NO_ALT_ENTER);
		}


		if (_attributes.DebugMode)
		{
			Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
			if (!FAILED(_d3d12Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
			{
				// Suppress whole categories of messages.
				//D3D12_MESSAGE_CATEGORY Categories[] = {};

				// Suppress messages based on their severity level.
				D3D12_MESSAGE_SEVERITY Severities[] =
				{
					D3D12_MESSAGE_SEVERITY_INFO
				};

				// Suppress individual messages by their ID.
				D3D12_MESSAGE_ID DenyIds[] =
				{
					// The 11On12 implementation does not use optimized clearing yet.
					D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
				};

				D3D12_INFO_QUEUE_FILTER NewFilter = {};
				//NewFilter.DenyList.NumCategories = _countof(Categories);
				//NewFilter.DenyList.pCategoryList = Categories;
				NewFilter.DenyList.NumSeverities = _countof(Severities);
				NewFilter.DenyList.pSeverityList = Severities;
				NewFilter.DenyList.NumIDs = _countof(DenyIds);
				NewFilter.DenyList.pIDList = DenyIds;

				pInfoQueue->PushStorageFilter(&NewFilter);
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			}
		}

		// Create the command queue
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = { }; // Use defaults
		if (FAILED(_d3d12Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&_commandQueue))))
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not create command queue." };
			return false;
		}
		_commandQueue->SetName(L"Renderer Command Queue");

		// Create the swap chain
		DXGI_MODE_DESC backBufferDesc = {}; 
		backBufferDesc.Width = _attributes.WindowWidth; 
		backBufferDesc.Height = _attributes.WindowHeight; 
		backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = frameBufferCount;
		swapChainDesc.Width = _attributes.WindowWidth;
		swapChainDesc.Height = _attributes.WindowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		if (!_attributes.VsyncOn || _allowTearing)
		{
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}
		//swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
		if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
			_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			window.GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&tempSwapChain
		)))
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not create swap chain." };
			return false;
		}

		// Need to cast to IDXGISwapChain3 for GetCurrentBackBufferIndex
		if (FAILED(tempSwapChain.As(&_swapChain)))
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not cast to IDXGISwapChain3." };
			return false;
		}
		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		// Create RTV Descriptor Heap -- //

		// describe an rtv descriptor heap and create
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = frameBufferCount; 
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; 
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		if (FAILED(_d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvDescriptorHeap))))
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not create RTV descriptor heap." };
			return false;
		}
		_rtvDescriptorHeap->SetName(L"RTV descriptor heap");
		_rtvDescriptorSize = _d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Handle to start of RTV heap
		CD3DX12_CPU_DESCRIPTOR_HANDLE  rtvHandle(_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each buffer
		for (int i = 0; i < frameBufferCount; i++)
		{
			if (FAILED(_swapChain->GetBuffer(i, IID_PPV_ARGS(_renderTargets[i].GetAddressOf()))))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not get buffer for RTV " + std::to_string(i) };
				return false;
			}
			_renderTargets[i]->SetName(L"FrameBuffer");

			// Create RTV
			_d3d12Device->CreateRenderTargetView(_renderTargets[i].Get(), nullptr, rtvHandle);

			// Increment the rtv handle by the rtv descriptor size
			rtvHandle.Offset(1, _rtvDescriptorSize);
		}

		// MS only makes 1
		// -- Create the Command Allocators -- //
		for (int i = 0; i < frameBufferCount; i++)
		{
			if (FAILED(_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator[i]))))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not create command allocator " + std::to_string(i) };
				return false;
			}
			_commandAllocator[i].Get()->SetName(ConvertToWide("Command Allocator " + std::to_string(i)).c_str());
		}

		// create the command list with the first allocator
		if (FAILED(_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator[0].Get(), NULL, IID_PPV_ARGS(&_commandList))))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create command list." };
			return false;
		}
		_commandList->SetName(L"CommandList");

		_commandList->Close();

		// MS creates only 1
		// Create the fences
		for (int i = 0; i < frameBufferCount; i++)
		{
			if (FAILED(_d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence[i]))))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not create fence " + std::to_string(i) };
				return false;
			}
			_fenceValue[i] = 0; // set the initial fence value to 0
		}

		// create a handle to a fence event
		_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (_fenceEvent == nullptr)
		{
			lastError = { GameErrors::GameDirectX12Specific, "Could not create fence event handle" };
			return false;
		}

		// Fill out the Viewport
		_viewPort.TopLeftX = 0;
		_viewPort.TopLeftY = 0;
		_viewPort.Width = (float_t)_attributes.WindowWidth;
		_viewPort.Height = (float_t)_attributes.WindowHeight;
		_viewPort.MinDepth = 0.0f;
		_viewPort.MaxDepth = 1.0f;

		// Fill out a scissor rect
		_scissorRect.left = 0;
		_scissorRect.top = 0;
		_scissorRect.right = _attributes.WindowWidth;
		_scissorRect.bottom = _attributes.WindowHeight;

		return true;
	};

	inline void RendererDX12::_WaitForPreviousFrame()
	{
		HRESULT hr;
// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
// sample illustrates how to use fences for efficient resource usage and to
// maximize GPU utilization.

		// Make sure we have the current back buffer being drawn to
		//if (nextFrame)
			_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		if (_fence[_frameIndex]->GetCompletedValue() < _fenceValue[_frameIndex])
		{
			// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
			hr = _fence[_frameIndex]->SetEventOnCompletion(_fenceValue[_frameIndex], _fenceEvent);
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Fence event completion failed." };
			}

			// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
			// has reached "fenceValue", we know the command queue has finished executing
			WaitForSingleObject(_fenceEvent, INFINITE);
		}

		// Increment the fence for next frame
		// and get the current back buffer again incase there was a swap
		//if (nextFrame)
		{
			_fenceValue[_frameIndex]++;
			_frameIndex = _swapChain->GetCurrentBackBufferIndex();
		}
	}

	inline void RendererDX12::Clear(const uint32_t bufferFlags, const Color color)
	{
		_StartFrame();
		_commandList->ClearRenderTargetView(_currentFrameBuffer, color.rgba, 0, nullptr);
	}

	inline void RendererDX12::_StartFrame()
	{

		if (_windowResized)
		{
			_DoWindowResize();
		}
		// We have to wait for the gpu to finish with the command allocator before we reset it
		_WaitForPreviousFrame();


		// Starting a frame, want to make sure we end it before closing application, dx12 can throw and error if
		// a command list is not used
		_midFrame = true;

		// RESET commandlist
		{
			// Reset this once per frame to free memory
			_commandAllocator[_frameIndex]->Reset();

			// reset the command list. 
			_commandList->Reset(_commandAllocator[_frameIndex].Get(), NULL);
		}

		// Transition current rendertarget to render target state
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_commandList->ResourceBarrier(1, &barrier);

		// Set the current render target
		_currentFrameBuffer = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		_currentFrameBuffer.Offset(_frameIndex, _rtvDescriptorSize);
		_commandList->OMSetRenderTargets(1, &_currentFrameBuffer, FALSE, nullptr);

		_commandList->RSSetViewports(1, &_viewPort);
		_commandList->RSSetScissorRects(1, &_scissorRect);
	}

	//inline void RendererDX12::EndFrame()
	//{
		//// Transition the render target to the present state.
		//CD3DX12_RESOURCE_BARRIER t = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		//_commandList->ResourceBarrier(1, &t);

		//_commandList->Close();

		//// EXECUTE
		//{
		//	ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };

		//	// execute the array of command lists
		//	_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		//	// this command goes in at the end of our command queue. we will know when our command queue 
		//	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
		//	// queue is being executed on the GPU
		//	_commandQueue->Signal(_fence[_frameIndex].Get(), _fenceValue[_frameIndex]);
		//}
	//}

	inline void RendererDX12::Swap()
	{
		// Transition the render target to the present state.
		CD3DX12_RESOURCE_BARRIER t = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		_commandList->ResourceBarrier(1, &t);

		_commandList->Close();

		// EXECUTE
		{
			ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };

			// execute the array of command lists
			_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			// this command goes in at the end of our command queue. we will know when our command queue 
			// has finished because the fence value will be set to "fenceValue" from the GPU since the command
			// queue is being executed on the GPU
			_commandQueue->Signal(_fence[_frameIndex].Get(), _fenceValue[_frameIndex]);
		}

		// Present the current rtv
		if (_attributes.VsyncOn)
		{
			_swapChain->Present(1, 0);
			// Below is needed for VSYNC to work for some reason
			flushGPU();
		}
		else
		{
			_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
		}
		_midFrame = false;
	}

	inline bool RendererDX12::LoadShader(const std::string vertex, const std::string fragment, Shader& shader)
	{
		if (!shader.isPrecompiled)
		{
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
			if (_attributes.DebugMode)
			{
				flags |= D3DCOMPILE_DEBUG;
				flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
			}
			else
			{
				//flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
				flags = 0;
			}
			Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;// = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;// = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> compilationMsgs;// = nullptr;

			// Compile the vertex shader
			if (FAILED(D3DCompileFromFile(ConvertToWide(vertex).c_str(), NULL, NULL, "main", "vs_5_0", flags, NULL, &compiledVertexShader, &compilationMsgs)))
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX12Specific,"Could not compile shader \"" + vertex + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				return false;
			}

			// Compile the pixel shader
			if (FAILED(D3DCompileFromFile(ConvertToWide(fragment).c_str(), NULL, NULL, "main", "ps_5_0", flags, NULL, &compiledPixelShader, &compilationMsgs)))
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX12Specific,"Could not compile shader \"" + fragment + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				return false;
			}
			
			// Cache the compiled code
			shader.compiledVertexShader12 = compiledVertexShader;
			shader.compiledPixelShader12 = compiledPixelShader;


			return true;
		}
		else
		{
			// Load compiled vertex shader
			Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;// = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;// = nullptr;

			// Load vertex shader
			if (FAILED(D3DReadFileToBlob((ConvertToWide(vertex).c_str()), &compiledVertexShader)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not read vertex file \"" + vertex + "\"." };
				return false;
			}

			// Load pixel shader
			if (FAILED(D3DReadFileToBlob((ConvertToWide(fragment).c_str()), &compiledPixelShader)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not read pixel file \"" + fragment + "\"." };
				return false;
			}

			// Cache the compiled code
			shader.compiledVertexShader12 = compiledVertexShader;
			shader.compiledPixelShader12 = compiledPixelShader;

		}
		return true;
	}

	inline bool RendererDX12::LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader)
	{
		if (!shader.isPrecompiled)
		{
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
			if (_attributes.DebugMode)
			{
				flags |= D3DCOMPILE_DEBUG;
				flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
			}
			else
			{
				//flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
				flags = 0;
			}
			Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;// = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;// = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> compilationMsgs;// = nullptr;

			// Compile the vertex shader
			if (FAILED(D3DCompile2(shaderText.c_str(), shaderText.length(), NULL, NULL, NULL, vertexEntryPoint.c_str(), "vs_5_0", flags, NULL, NULL, NULL, NULL, compiledVertexShader.GetAddressOf(), compilationMsgs.GetAddressOf())))
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX12Specific,"Could not compile vertex shader from \"" + shaderText + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				return false;
			}

			// Compile the pixel shader
			if (FAILED(D3DCompile2(shaderText.c_str(), shaderText.length(), NULL, NULL, NULL, fragmentEntryPoint.c_str(), "ps_5_0", flags, NULL, NULL, NULL, NULL, compiledPixelShader.GetAddressOf(), compilationMsgs.GetAddressOf())))
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX12Specific,"Could not compile pixel shader from \"" + shaderText + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				return false;
			}

			// Cache the compiled code
			shader.compiledVertexShader12 = compiledVertexShader;
			shader.compiledPixelShader12 = compiledPixelShader;


			return true;
		}
		else
		{
			//// Load compiled vertex shader
			//Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;// = nullptr;
			//Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;// = nullptr;

			//// Load vertex shader
			//if (FAILED(D3DReadFileToBlob((ConvertToWide(vertex).c_str()), &compiledVertexShader)))
			//{
			//	lastError = { GameErrors::GameDirectX11Specific,"Could not read vertex file \"" + vertex + "\"." };
			//	return false;
			//}

			//// Load pixel shader
			//if (FAILED(D3DReadFileToBlob((ConvertToWide(fragment).c_str()), &compiledPixelShader)))
			//{
			//	lastError = { GameErrors::GameDirectX11Specific,"Could not read pixel file \"" + fragment + "\"." };
			//	return false;
			//}

			//// Cache the compiled code
			//shader.compiledVertexShader12 = compiledVertexShader;
			//shader.compiledPixelShader12 = compiledPixelShader;

		}
		return true;
	}

	inline void RendererDX12::UnLoadShader(Shader& shader)
	{

	}

	inline bool RendererDX12::CreateTexture(Texture2D& texture) {
		texture.oneOverWidth = 1.0f / (float_t)texture.width;
		texture.oneOverHeight = 1.0f / (float_t)texture.height;
		texture.isCopy = false;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = texture.width;
		textureDesc.Height = texture.height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = _d3d12Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&texture.textureResource12));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create texture " + texture.name + " heap." };
			AppendHR12(hr);
			return false;
		}
		texture.textureResource12.Get()->SetName(ConvertToWide(texture.name.c_str()).c_str());

		// Create the upload heap for the texture
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.textureResource12.Get(), 0, 1);
		heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		hr = _d3d12Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texture.textureUploadHeap12));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create texture " + texture.name + " upload heap." };
			AppendHR12(hr);
			return false;
		}

		// Describe and create a SRV heap for the texture.
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		hr = _d3d12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&texture.srvHeap));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create texture " + texture.name + " srv heap." };
			AppendHR12(hr);
			return false;
		}

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_d3d12Device->CreateShaderResourceView(texture.textureResource12.Get(), &srvDesc, texture.srvHeap->GetCPUDescriptorHandleForHeapStart());
		
		return true;
	};

	inline bool RendererDX12::LoadTexture(std::string fileName, Texture2D& texture) 
	{
		ImageLoader loader;
		void* data = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t componentsPerPixel = 0;

		data = loader.Load(fileName.c_str(), width, height, componentsPerPixel);
		if (data == nullptr)
		{
			lastError = { GameErrors::GameContent, "Failed to load texture : " + fileName };
			return false;
		}

		texture.width = width;
		texture.height = height;
		texture.oneOverWidth = 1.0f / (float_t)texture.width;
		texture.oneOverHeight = 1.0f / (float_t)texture.height;
		texture.isCopy = false;
		texture.name = fileName;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = texture.width;
		textureDesc.Height = texture.height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HRESULT hr = _d3d12Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&texture.textureResource12));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create texture " + texture.name + " heap." };
			AppendHR12(hr);
			return false;
		}
		texture.textureResource12.Get()->SetName(ConvertToWide(texture.name.c_str()).c_str());

		// Create the upload heap for the texture
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.textureResource12.Get(), 0, 1);
		heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		hr = _d3d12Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texture.textureUploadHeap12));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create texture " + texture.name + " upload heap." };
			AppendHR12(hr);
			return false;
		}

		// Describe and create a SRV heap for the texture.
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = _d3d12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&texture.srvHeap));
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"Could not create texture " + texture.name + " srv heap." };
			AppendHR12(hr);
			return false;
		}

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_d3d12Device->CreateShaderResourceView(texture.textureResource12.Get(), &srvDesc, texture.srvHeap->GetCPUDescriptorHandleForHeapStart());

		// Upload it here 
		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = reinterpret_cast<uint8_t*>(data);
		textureData.RowPitch = static_cast<int64_t>(texture.width) * 4;
		textureData.SlicePitch = 0;

		_commandList->Reset(_commandAllocator->Get(), NULL);
		CD3DX12_RESOURCE_BARRIER trans = CD3DX12_RESOURCE_BARRIER::Transition(texture.textureResource12.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
		_commandList->ResourceBarrier(1, &trans);
		UpdateSubresources(_commandList.Get(), texture.textureResource12.Get(), texture.textureUploadHeap12.Get(), 0, 0, 1, &textureData);
		trans = CD3DX12_RESOURCE_BARRIER::Transition(texture.textureResource12.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		_commandList->ResourceBarrier(1, &trans);

		_commandList->Close();

		ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };

		// execute the array of command lists
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		_fenceValue[_frameIndex]++;
		hr = _commandQueue->Signal(_fence[_frameIndex].Get(), _fenceValue[_frameIndex]);
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX12Specific,"LoadTexture signal failed." };
			AppendHR12(hr);
			return false;
		}
		flushGPU();

		//lastError = { GameErrors::GameDirectX12Specific,"Texture not implemented " }; 
		return true;
	};
}

#undef frameBufferCount