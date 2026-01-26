#if !defined(GAMERENDERERDX11_H)
#define GAMERENDERERDX11_H
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <dxgitype.h>
#include <d3dcommon.h>
#include <Windows.h>
#include <wrl/client.h>
#include <cmath>
#include <cstdint>
#include <string>
#include "GameErrors.h"
#include "GameImageLoader.h"
#include "GameRendererBase.h"
#include "GameShader.h"
#include "GameSystemInfo.h"
#include "GameTexture2D.h"
#include "Game.h"
#include "GameColor.h"
#include "GameHelpers.h"
#include "GameWindow.h"


namespace game
{
	extern SystemInfo systemInfo;
	class RendererDX11 : public RendererBase
	{
	public:
		RendererDX11();
		bool CreateDevice(Window& window);
		void DestroyDevice();
		void Swap();
		void HandleWindowResize(const uint32_t width, const uint32_t height);
		void FillOutRendererInfo() {}
		bool CreateTexture(Texture2D& texture);
		bool LoadTexture(const std::string& fileName, Texture2D& texture);
		void UnLoadTexture(Texture2D& texture);
		bool LoadShader(const std::string vertex, const std::string fragment, Shader& shader);
		bool LoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader)
		{
			lastError = { GameErrors::GameDirectX11Specific, "Geometry shaders not implemented yet." };
			return false;
		}
		bool LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader);
		void UnLoadShader(Shader& shader);
		void GetDevice(Microsoft::WRL::ComPtr<ID3D11Device>&device, Microsoft::WRL::ComPtr <ID3D11DeviceContext>& context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& target, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depth);
		void Clear(const uint32_t bufferFlags, const Color color);
	protected:
		void _ReadExtensions() {};
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> _d3d11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _d3d11DeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> _d3d11SwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _d3d11RenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _d3d11DepthStencilView;
	};

	inline RendererDX11::RendererDX11()
	{
		//_d3d11Device = nullptr;
		//_d3d11DeviceContext = nullptr;
		//_d3d11SwapChain = nullptr;
		//_d3d11RenderTargetView = nullptr;
		//_d3d11DepthStencilView = nullptr;
		//_d3d11DepthStencilBuffer = nullptr;
	}

	inline void RendererDX11::Clear(const uint32_t bufferFlags, const Color color)
	{
		_d3d11DeviceContext->ClearRenderTargetView(_d3d11RenderTargetView.Get(), color.rgba);
		_d3d11DeviceContext->ClearDepthStencilView(_d3d11DepthStencilView.Get(), bufferFlags, 1.0f, 0);
	}

	inline bool RendererDX11::CreateDevice(Window& window) 
	{ 
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,	// 0xb100
			D3D_FEATURE_LEVEL_11_0, // 0xb00
			D3D_FEATURE_LEVEL_10_1, // 0xa100
			D3D_FEATURE_LEVEL_10_0, // 0xa000
			D3D_FEATURE_LEVEL_9_3,	// 0x9300
			D3D_FEATURE_LEVEL_9_2,	// 0x9200
			D3D_FEATURE_LEVEL_9_1,	// 0x9100
		};
		uint32_t deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D3D_FEATURE_LEVEL featureLevelCreated;
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		D3D11_VIEWPORT viewPort = { 0 };
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		//uint32_t debug = 0; // not used
		D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
		Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11DepthStencilBuffer;

		// back buffer stuff
		scd.BufferDesc.Width = _attributes.WindowWidth;
		scd.BufferDesc.Height = _attributes.WindowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 0; // 60
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// multisampling
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;

		scd.OutputWindow = window.GetHandle();
		scd.Windowed = true;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING for variable refresh rate, needs DXGI_SWAP_EFFECT_FLIP_DISCARD

		if (_attributes.DebugMode)
		{
			deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		// Create device and swap chain
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &scd, _d3d11SwapChain.GetAddressOf(), _d3d11Device.GetAddressOf(), &featureLevelCreated, _d3d11DeviceContext.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX10Specific, "Could not create device." };
			return false;
		}

		// Describe depth stencil
		depthStencilDesc.Width = _attributes.WindowWidth;
		depthStencilDesc.Height = _attributes.WindowHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;	// can change for reading or writing i think
		depthStencilDesc.MiscFlags = 0;
		
		// Create depth stencil buffer texture
		if (FAILED(_d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, d3d11DepthStencilBuffer.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create depth stencil buffer texture." };
			DestroyDevice();
			return false;
		}

		// Second param is states for depth stencil
		if (FAILED(_d3d11Device->CreateDepthStencilView(d3d11DepthStencilBuffer.Get(), NULL, &_d3d11DepthStencilView)))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create depth stencil view." };
			return false;
		}

		HRESULT hr;
		if (FAILED(_d3d11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create create back buffer." };
			return false;
		}

		//Create our Render Target
		hr = _d3d11Device->CreateRenderTargetView(backBuffer.Get(), NULL, _d3d11RenderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create render target view." };
			return false;
		}

		//Set our Render Target
		_d3d11DeviceContext->OMSetRenderTargets(1, _d3d11RenderTargetView.GetAddressOf(), _d3d11DepthStencilView.Get());

		// Set the viewport
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.Width = (float_t)_attributes.WindowWidth;
		viewPort.Height = (float_t)_attributes.WindowHeight;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;

		_d3d11DeviceContext->RSSetViewports(1, &viewPort);

		
		return true; 
	}
	
	inline void RendererDX11::GetDevice(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr <ID3D11DeviceContext>& context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& target, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depth)
	{
		device = _d3d11Device;
		context = _d3d11DeviceContext;
		target = _d3d11RenderTargetView;
		depth = _d3d11DepthStencilView;
	}

	inline void RendererDX11::DestroyDevice()
	{
		_d3d11Device.Reset();
		_d3d11DeviceContext.Reset();
		_d3d11SwapChain.Reset();
		_d3d11RenderTargetView.Reset();
		//_d3d11DepthStencilBuffer.Reset();
		_d3d11DepthStencilView.Reset();
	}

	inline void RendererDX11::Swap()
	{
		if (_attributes.VsyncOn)
		{
			_d3d11SwapChain->Present(1, 0); // first is vsync, 0 for non, 1-4 interval, second is ???
		}
		else
		{
			_d3d11SwapChain->Present(0, 0);
		}
	}

	inline void RendererDX11::HandleWindowResize(const uint32_t width, const uint32_t height)
	{
		D3D11_VIEWPORT viewPort = { 0 };
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
		Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11DepthStencilBuffer;

		if (!width || !height) return;

		// Save new size
		_attributes.WindowWidth = width;
		_attributes.WindowHeight = height;

		// Destory old buffers
		_d3d11DeviceContext->OMSetRenderTargets(NULL, NULL, NULL);
		_d3d11RenderTargetView.Reset();
		_d3d11DepthStencilView.Reset();
		_d3d11DeviceContext->Flush();

		// Resize the new buffers
		_d3d11SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);

		// Create depth and stencil buffer
		depthStencilDesc.Width = _attributes.WindowWidth;
		depthStencilDesc.Height = _attributes.WindowHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;	// can change for reading or writing i think
		depthStencilDesc.MiscFlags = 0;

		// Create depth stencil buffer texture
		if (FAILED(_d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, d3d11DepthStencilBuffer.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create depth stencil buffer texture on resize." };
			DestroyDevice();
			return;
		}

		// Second param is states for depth stencil
		if (FAILED(_d3d11Device->CreateDepthStencilView(d3d11DepthStencilBuffer.Get(), NULL, &_d3d11DepthStencilView)))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create depth stencil view on resize." };
			DestroyDevice();
			return;
		}

		// Create the back buffer texture
		if (FAILED(_d3d11SwapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()))))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create back buffer on resize." };
			DestroyDevice();
			return;
		}

		// Create the render target
		if (FAILED(_d3d11Device->CreateRenderTargetView(backBuffer.Get(), 0, _d3d11RenderTargetView.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create render target on resize." };
			DestroyDevice();
			return;
		}

		// Set the render target
		_d3d11DeviceContext->OMSetRenderTargets(1, _d3d11RenderTargetView.GetAddressOf(), _d3d11DepthStencilView.Get());

		// Set the viewport
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.Width = (float_t)_attributes.WindowWidth;
		viewPort.Height = (float_t)_attributes.WindowHeight;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		_d3d11DeviceContext->RSSetViewports(1, &viewPort);
	}

	inline bool RendererDX11::LoadTexture(const std::string& fileName, Texture2D& texture)
	{
		ImageLoader loader;
		void* data = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t componentsPerPixel = 0;
		//D3D11_MAPPED_TEXTURE2D  lockedRectangle = { 0 };

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

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = texture.width;
		desc.Height = texture.height;
		desc.MipLevels = texture.isMipMapped ? 0 : 1;// 0 for mipmaps, 1 for not
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;// D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;// D3D10_CPU_ACCESS_WRITE;
		if (texture.isMipMapped)
		{
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		// Create texture memory
		if (FAILED(_d3d11Device->CreateTexture2D(&desc, NULL, texture.textureInterface11.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create texture, \"" + fileName + "\"." };
			return false;
		}

		// Copy data into memory
		_d3d11DeviceContext->UpdateSubresource(texture.textureInterface11.Get(), 0, NULL, data, desc.Width * sizeof(uint8_t) * 4, 0);

		// Create SRV for the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc = {};
		srDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = texture.isMipMapped ? -1 : 1; // -1 for mipmaps
		if (FAILED(_d3d11Device->CreateShaderResourceView(texture.textureInterface11.Get(), &srDesc, texture.textureSRV11.GetAddressOf())))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create texture SRV, \"" + fileName + "\"." };
			return false;
		}
		if (texture.isMipMapped)
			_d3d11DeviceContext->GenerateMips(texture.textureSRV11.Get());
		//_d3d10Device->GenerateMips(texture.textureSRV10.Get());
		texture.isLoaded = true;
		return true;
	}

	inline bool RendererDX11::CreateTexture(Texture2D& texture)
	{
		texture.oneOverWidth = 1.0f / (float_t)texture.width;
		texture.oneOverHeight = 1.0f / (float_t)texture.height;
		texture.isCopy = false;

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = texture.width;
		desc.Height = texture.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		if (FAILED(_d3d11Device->CreateTexture2D(&desc, NULL, &texture.textureInterface11)))
		{
			lastError = { GameErrors::GameDirectX11Specific, "Could not create texture." };
			return false;
		}
		

		return true;
	}

	inline void RendererDX11::UnLoadTexture(Texture2D& texture)
	{
		texture.textureInterface11.Reset();
		texture.textureSRV11.Reset();
		texture.width = 0;
		texture.height = 0;
		texture.oneOverWidth = 0.0f;
		texture.oneOverHeight = 0.0f;
		texture.componentsPerPixel = 0;
		texture.isCopy = false;
		texture.name = "NULL";
		// Attributes of texture filtering
		texture.isMipMapped = true;
		texture.filterType = TextureFilterType::Trilinear;
		texture.anisotropyLevel = 1;
	}

	inline bool RendererDX11::LoadShader(const std::string vertex, const std::string fragment, Shader& shader)
	{
		if (!shader.isPrecompiled)
		{
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
			if (_attributes.DebugMode)
			{
				flags |= D3DCOMPILE_DEBUG;
			}
			Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;
			Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;
			Microsoft::WRL::ComPtr<ID3DBlob> compilationMsgs;

			// Compile the vertex shader
			if (FAILED(D3DCompileFromFile(ConvertToWide(vertex).c_str(), NULL, NULL, "main", "vs_4_0", flags, NULL, &compiledVertexShader, &compilationMsgs)))
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX11Specific,"Could not compile shader \"" + vertex + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				return false;
			}

			// Compile the pixel shader
			if (FAILED(D3DCompileFromFile(ConvertToWide(fragment).c_str(), NULL, NULL, "main", "ps_4_0", flags, NULL, &compiledPixelShader, &compilationMsgs)))
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				auto* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX11Specific,"Could not compile shader \"" + fragment + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				return false;
			}

			// Create vertex shader
			if (FAILED(_d3d11Device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &shader.vertexShader11)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create vertex shader from \"" + vertex + "\"." };
				return false;
			}

			// Create pixel shader
			if (FAILED(_d3d11Device->CreatePixelShader((DWORD*)(compiledPixelShader->GetBufferPointer()), compiledPixelShader->GetBufferSize(), NULL, &shader.pixelShader11)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create pixel shader from \"" + fragment + "\"." };
				return false;
			}

			// Shaders created, save a reference and release this one
			shader.compiledVertexShader11 = compiledVertexShader;
			shader.compiledPixelShader11 = compiledPixelShader;

			return true;
		}
		else
		{
			// Load compiled vertex shader
			Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;
			Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;

			// Create vertex shader
			if (FAILED(D3DReadFileToBlob((ConvertToWide(vertex).c_str()), &compiledVertexShader)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not read vertex file \"" + vertex + "\"." };
				return false;
			}
			HRESULT hr = _d3d11Device->CreateVertexShader((DWORD*)compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &shader.vertexShader11);
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create vertex shader from \"" + vertex + "\"." };
				return false;
			}

			// Create pixel shader
			if (FAILED(D3DReadFileToBlob((ConvertToWide(fragment).c_str()), &compiledPixelShader)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not read pixel file \"" + fragment + "\"." };
			}

			if (FAILED(_d3d11Device->CreatePixelShader((DWORD*)compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &shader.pixelShader11)))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create pixel shader from \"" + fragment + "\"." };
				return false;
			}

			// Shaders created, save a reference and release this one
			shader.compiledVertexShader11 = compiledVertexShader;
			shader.compiledPixelShader11 = compiledPixelShader;

		}
		return true;
	}

	inline bool RendererDX11::LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader)
	{
		//if (FAILED(D3DCompile2(shaderText.c_str(), shaderText.length(), NULL, NULL, NULL, vertexEntryPoint.c_str(), "vs_5_0", flags, NULL, NULL, NULL, NULL, compiledVertexShader.GetAddressOf(), compilationMsgs.GetAddressOf())))


		DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
		if (_attributes.DebugMode)
		{
			flags |= D3DCOMPILE_DEBUG;
		}
		Microsoft::WRL::ComPtr<ID3DBlob> compiledVertexShader;
		Microsoft::WRL::ComPtr<ID3DBlob> compiledPixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob> compilationMsgs;

		// Compile the vertex shader
		//if (FAILED(D3DCompileFromFile(ConvertToWide(vertex).c_str(), NULL, NULL, "main", "vs_4_0", flags, NULL, &compiledVertexShader, &compilationMsgs)))
		if (FAILED(D3DCompile2(shaderText.c_str(), shaderText.length(), NULL, NULL, NULL, vertexEntryPoint.c_str(), "vs_4_0", flags, NULL, NULL, NULL, NULL, compiledVertexShader.GetAddressOf(), compilationMsgs.GetAddressOf())))
		{
			SIZE_T size = compilationMsgs->GetBufferSize();
			uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
			lastError = { GameErrors::GameDirectX11Specific,"Could not compile vertex shader from \"" + shaderText + "\".\n" };
			for (uint32_t bytes = 0; bytes < size; bytes++)
			{
				lastError.lastErrorString += p[bytes];
			}
			return false;
		}

		// Compile the pixel shader
		//if (FAILED(D3DCompileFromFile(ConvertToWide(fragment).c_str(), NULL, NULL, "main", "ps_4_0", flags, NULL, &compiledPixelShader, &compilationMsgs)))
		if (FAILED(D3DCompile2(shaderText.c_str(), shaderText.length(), NULL, NULL, NULL, fragmentEntryPoint.c_str(), "ps_4_0", flags, NULL, NULL, NULL, NULL, compiledPixelShader.GetAddressOf(), compilationMsgs.GetAddressOf())))
		{
			SIZE_T size = compilationMsgs->GetBufferSize();
			auto* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
			lastError = { GameErrors::GameDirectX11Specific,"Could not compile pixel shader from \"" + shaderText + "\".\n" };
			for (uint32_t bytes = 0; bytes < size; bytes++)
			{
				lastError.lastErrorString += p[bytes];
			}
			return false;
		}

		// Create vertex shader
		if (FAILED(_d3d11Device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &shader.vertexShader11)))
		{
			lastError = { GameErrors::GameDirectX11Specific,"Could not create vertex shader from \"" + shaderText + "\"." };
			return false;
		}

		// Create pixel shader
		if (FAILED(_d3d11Device->CreatePixelShader((DWORD*)(compiledPixelShader->GetBufferPointer()), compiledPixelShader->GetBufferSize(), NULL, &shader.pixelShader11)))
		{
			lastError = { GameErrors::GameDirectX11Specific,"Could not create pixel shader from \"" + shaderText + "\"." };
			return false;
		}

		// Shaders created, save a reference and release this one
		shader.compiledVertexShader11 = compiledVertexShader;
		shader.compiledPixelShader11 = compiledPixelShader;

		return true;
	}

	inline void RendererDX11::UnLoadShader(Shader& shader)
	{
		// Maybe should be resets?
		//SAFE_RELEASE(shader.compiledVertexShader11);
		//SAFE_RELEASE(shader.vertexShader11);
		//SAFE_RELEASE(shader.compiledPixelShader11);
		//SAFE_RELEASE(shader.pixelShader11);
		//SAFE_RELEASE(shader.compiledGeometryShader11);
		//SAFE_RELEASE(shader.geometryShader11);
	}
}

#endif