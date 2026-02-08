#if !defined(GAMERENDERERDX9_H)
#define GAMERENDERERDX9_H

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = nullptr; } }
#endif

#include <d3d9.h>
#include <d3dcompiler.h>
#include <fstream>
//#include <dxgi.h>
//#pragma comment(lib, "dxgi.lib") // for gpu memory

#include "GameErrors.h"
#include "GameHelpers.h"
#include "GameImageLoader.h"
#include "GameRendererBase.h"
#include "GameShader.h"
#include "GameSystemInfo.h"
#include "GameTexture2D.h"
#include "GameTextShaders.h"

namespace game
{
	extern SystemInfo systemInfo;
	class RendererDX9 : public RendererBase
	{
	public:
		RendererDX9();
		bool CreateDevice(Window& window);
		void DestroyDevice();
		void Swap();
		void HandleWindowResize(const uint32_t width, const uint32_t height);
		void FillOutRendererInfo();
		bool CreateTexture(Texture2D& texture);
		bool LoadTexture(const std::string& fileName, Texture2D& texture);
		void UnLoadTexture(Texture2D& texture);
		bool LoadShader(const std::string vertex, const std::string fragment, Shader& shader);
		bool LoadShader(const std::string vertex, const std::string fragment, const std::string geometry, Shader& shader)
		{
			lastError = { GameErrors::GameDirectX9Specific, "Geometry shaders are not supported." };
			return false;
		}
		bool LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader& shader)
		{
			//bool LoadTextShader(const std::string shaderText, const std::string vertexEntryPoint, const std::string fragmentEntryPoint, Shader & shader)
			//{
			//if (FAILED(D3DCompile2(shaderText.c_str(), shaderText.length(), NULL, NULL, NULL, vertexEntryPoint.c_str(), "vs_5_0", flags, NULL, NULL, NULL, NULL, compiledVertexShader.GetAddressOf(), compilationMsgs.GetAddressOf())))
			//return false;
			//}
			return false;
		}
		void UnLoadShader(Shader& shader);
		void GetDevice(LPDIRECT3DDEVICE9& device);
		void Clear(const uint32_t bufferFlags, const Color color);
	protected:
		void _ReadExtensions() {};
	private:
		LPDIRECT3D9 _d3d9;
		LPDIRECT3DDEVICE9 _d3d9Device;
		D3DPRESENT_PARAMETERS _d3dpp = { 0 };


	};

	inline RendererDX9::RendererDX9()
	{
		_d3d9 = nullptr;
		_d3d9Device = nullptr;

	}

	inline void RendererDX9::GetDevice(LPDIRECT3DDEVICE9& device)
	{
		_d3d9Device->AddRef();
		device = _d3d9Device;
	}

	inline void RendererDX9::HandleWindowResize(const uint32_t width, const uint32_t height)
	{
		D3DVIEWPORT9 view { 0, 0, width, height, 0.0f, 1.0f};
		
		if (!width || !height) return;

		_d3d9Device->Clear(0, NULL, D3DCLEAR_TARGET, Colors::Black.packedARGB, 1.0f, 0);
		_d3dpp.BackBufferWidth = width;
		_d3dpp.BackBufferHeight = height;
		_d3d9Device->Reset(&_d3dpp);
		_d3d9Device->SetViewport(&view);
	}

	inline void RendererDX9::DestroyDevice()
	{
		SAFE_RELEASE(_d3d9Device);
		SAFE_RELEASE(_d3d9);
	}

	inline bool RendererDX9::CreateDevice(Window& window)
	{
		D3DPRESENT_PARAMETERS d3dpp = { 0 };   
		DWORD multiSamplingQuality = 0;
		
		_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);    

		// Setup present parameters
		d3dpp.Windowed = !_attributes.WindowFullscreen;    
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    
		d3dpp.hDeviceWindow = window.GetHandle();   
		d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		d3dpp.EnableAutoDepthStencil = true;
		//D3DFMT_D16_LOCKABLE = 70,
		//	D3DFMT_D32 = 71,
		//	D3DFMT_D15S1 = 73,
		//	D3DFMT_D24S8 = 75,
		//	D3DFMT_D24X8 = 77,
		//	D3DFMT_D24X4S4 = 79,
		//	D3DFMT_D16 = 80,

		//	D3DFMT_D32F_LOCKABLE = 82,
		//	D3DFMT_D24FS8 = 83,
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8; // 24 bit, no stencil
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		if (_attributes.VsyncOn)
		{
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		// Enables multisampling
		if (_attributes.MultiSamples > 1)
		{
			if (FAILED(_d3d9->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, !_attributes.WindowFullscreen, (D3DMULTISAMPLE_TYPE)_attributes.MultiSamples, &multiSamplingQuality)))
			{
				lastError = { GameErrors::GameDirectX9Specific, "Device does not support " + std::to_string(_attributes.MultiSamples) + "x multisampling."};
				return false;
			}
			d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)_attributes.MultiSamples;
			d3dpp.MultiSampleQuality = multiSamplingQuality - 1;
		}
		_d3dpp = d3dpp;
		// Create the device
		if (FAILED(_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window.GetHandle(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_d3d9Device)))
		{
			lastError = { GameErrors::GameDirectX9Specific, "Create Device failed." };
			return false;
		}
		D3DVIEWPORT9 view{ 0, 0, _attributes.WindowWidth, _attributes.WindowHeight, 0.0f, 1.0f };

		_d3d9Device->SetViewport(&view);

		// If device was created then the settings must have worked, log them
		systemInfo.gpuInfo.depthBufferSize = 24; //temp
		systemInfo.gpuInfo.backBufferColorSize = 32; // temp
		systemInfo.gpuInfo.frontBufferColorSize = 32; //temp
		systemInfo.gpuInfo.multisampleSamples = _attributes.MultiSamples;
		systemInfo.gpuInfo.maxMultisamples = 0; // temp till I can fix
		systemInfo.gpuInfo.internalPixelFormat = (int32_t)D3DFMT_A8R8G8B8;// figure out from somewhere
		systemInfo.gpuInfo.internalPixelType = 0; // unkown
		return true;
	}

	inline void RendererDX9::FillOutRendererInfo()
	{
		std::stringstream sStream;
		std::string tempValue;
		D3DADAPTER_IDENTIFIER9 adapterIdentifier = { 0 };

		// Lambda to log easier
		auto LOG = [&](std::stringstream& stream)
		{
			_logger->Write(stream.str());
			stream.str("");
		};

		// Log the OpenGL rendering device
		_d3d9->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifier);
		systemInfo.gpuInfo.renderer = adapterIdentifier.Description;
		sStream << "DirectX9 renderer : " << systemInfo.gpuInfo.renderer;
		LOG(sStream);

		// Log the version of DirectX context
		systemInfo.gpuInfo.version = "DirectX 9.0c";
		sStream << "DirectX version : " << systemInfo.gpuInfo.version;
		LOG(sStream);

		D3DCAPS9 caps;
		_d3d9Device->GetDeviceCaps(&caps);

		// Log shader versions
		systemInfo.gpuInfo.maxShaderLanguageVersion = "VS 3.0, ";
		if (caps.VertexShaderVersion < D3DVS_VERSION(3, 0))
		{
			systemInfo.gpuInfo.maxShaderLanguageVersion = "VS 2.x, ";
			if (caps.VertexShaderVersion < D3DVS_VERSION(2, 0))
			{
				systemInfo.gpuInfo.maxShaderLanguageVersion = "VS 1.1, ";
			}
		}
		systemInfo.gpuInfo.maxShaderLanguageVersion += "PS 3.0";
		if (caps.PixelShaderVersion < D3DVS_VERSION(3, 0))
		{
			systemInfo.gpuInfo.maxShaderLanguageVersion += "PS 2.x";
			if (caps.PixelShaderVersion < D3DVS_VERSION(2, 0))
			{
				systemInfo.gpuInfo.maxShaderLanguageVersion += "PS 1.1";
			}
		}
		sStream << "Max supported shader versions : " << systemInfo.gpuInfo.maxShaderLanguageVersion;
		LOG(sStream);

		// Log video memory
		systemInfo.gpuInfo.freeMemory = _d3d9Device->GetAvailableTextureMem() / 1024 / 1024;
		sStream << "GPU available memory : " << systemInfo.gpuInfo.freeMemory << "MB";
		LOG(sStream);

		// Log internal pixel format
		sStream << "Internal Pixel Format : " << systemInfo.gpuInfo.internalPixelFormat << " (";
		switch (systemInfo.gpuInfo.internalPixelFormat)
		{
		case 0: sStream << "Unknown)"; break;
		case 21: sStream << "ARGB)"; break;
		default: sStream << "Unknown)";
		}
		LOG(sStream);

		// Log internal pixel type
		sStream << "Internal Pixel Type : " << systemInfo.gpuInfo.internalPixelType << " (";
		switch (systemInfo.gpuInfo.internalPixelType)
		{
		case 0: sStream << "Unknown)"; break;
		default: sStream << "Unknown)";
		}
		LOG(sStream);

		// Log multisampling 
		sStream << "Multisampling samples : ";
		if (systemInfo.gpuInfo.multisampleSamples > 1)
		{
			sStream << systemInfo.gpuInfo.multisampleSamples << " (";
			sStream << systemInfo.gpuInfo.maxMultisamples << " max)";
		}
		else
		{
			sStream << "Disabled";
		}
		LOG(sStream);

		// Log front buffer
		sStream << "Front buffer : " << systemInfo.gpuInfo.frontBufferColorSize << " bits";
		LOG(sStream);

		// Log back buffer
		sStream << "Back buffer : " << systemInfo.gpuInfo.backBufferColorSize << " bits";
		LOG(sStream);

		// Log depth buffer
		sStream << "Depth buffer : " << systemInfo.gpuInfo.depthBufferSize << " bits";
		LOG(sStream);

		// Log max anisotropy
		systemInfo.gpuInfo.maxAnisotropy = caps.MaxAnisotropy;
		sStream << "Max anisotropy : " << systemInfo.gpuInfo.maxAnisotropy << "x";
		LOG(sStream);
	}
		
	inline void RendererDX9::Swap()
	{
		_d3d9Device->EndScene();
		_d3d9Device->Present(NULL, NULL, NULL, NULL);
	}

	inline void RendererDX9::Clear(const uint32_t bufferFlags, const Color color)
	{
		_d3d9Device->BeginScene();
		//if (bufferFlags & GAME_FRAME_BUFFER_BIT)
		//{
		_d3d9Device->Clear(0, NULL, bufferFlags, color.packedARGB, 1.0f, 0);
		//}
		//D3DCLEAR_STENCIL	Clear the stencil buffer.
		//D3DCLEAR_TARGET	Clear a render target, or all targets in a multiple render target.See Multiple Render Targets(Direct3D 9).
		//D3DCLEAR_ZBUFFER	Clear the depth buffer.

	}

	inline bool RendererDX9::CreateTexture(Texture2D& texture)
	{
		texture.oneOverWidth = 1.0f / (float_t)texture.width;
		texture.oneOverHeight = 1.0f / (float_t)texture.height;
		texture.isCopy = false;

		// does mipmapping, need to add format also
		_d3d9Device->CreateTexture(texture.width, texture.height, texture.isMipMapped ? 0 : 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture.textureInterface9, NULL);
		if (!texture.textureInterface9)
		{
			lastError = { GameErrors::GameDirectX9Specific, "Could not create texture." };
			return false;
		}

		return true;
	}

	inline bool RendererDX9::LoadTexture(const std::string& fileName, Texture2D& texture)
	{
		ImageLoader loader;
		void* data = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t componentsPerPixel = 0;
		D3DLOCKED_RECT lockedRectangle = { 0 };
		HRESULT hResult;
		
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

		// Create texture memory
		// hResult = _d3d9Device->CreateTexture(texture.width, texture.height, 1, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture.textureInterface9, NULL);
		hResult = _d3d9Device->CreateTexture(texture.width, texture.height, texture.isMipMapped ? 0 : 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture.textureInterface9, NULL);
		if (hResult != D3D_OK)
		{
			lastError = { GameErrors::GameDirectX9Specific, "Could not create texture, \"" + fileName + "\""};
		}

		// Copy texture data to the memory
		texture.textureInterface9->LockRect(0, &lockedRectangle, 0, 0);
		unsigned char* dest = static_cast<unsigned char*>(lockedRectangle.pBits);
		if (dest != NULL)
		{
			memcpy(dest, data, sizeof(unsigned char) * texture.width * texture.height * 4);
		}
		texture.textureInterface9->UnlockRect(0);
		texture.isLoaded = true;
		return true;
	}

	inline void RendererDX9::UnLoadTexture(Texture2D& texture)
	{
		SAFE_RELEASE(texture.textureInterface9);
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

	inline bool RendererDX9::LoadShader(const std::string vertex, const std::string fragment, Shader& shader)
	{
		if (!shader.isPrecompiled)
		{
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
			if (_attributes.DebugMode)
			{
				flags |= D3DCOMPILE_DEBUG;
			}
			ID3DBlob* compiledVertexShader = nullptr;
			ID3DBlob* compiledPixelShader = nullptr;
			ID3DBlob* compilationMsgs = nullptr;

			// Compile the vertex shader
			if (D3DCompileFromFile(ConvertToWide(vertex).c_str(), NULL, NULL, "main", "vs_3_0", flags, NULL, &compiledVertexShader, &compilationMsgs) != D3D_OK)
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				uint8_t* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX9Specific,"Could not compile shader \"" + vertex + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				SAFE_RELEASE(compilationMsgs);
				SAFE_RELEASE(compiledVertexShader);
				return false;
			}

			// Compile the pixel shader
			if (D3DCompileFromFile(ConvertToWide(fragment).c_str(), NULL, NULL, "main", "ps_3_0", flags, NULL, &compiledPixelShader, &compilationMsgs) != D3D_OK)
			{
				SIZE_T size = compilationMsgs->GetBufferSize();
				auto* p = reinterpret_cast<unsigned char*>(compilationMsgs->GetBufferPointer());
				lastError = { GameErrors::GameDirectX9Specific,"Could not compile shader \"" + fragment + "\".\n" };
				for (uint32_t bytes = 0; bytes < size; bytes++)
				{
					lastError.lastErrorString += p[bytes];
				}
				SAFE_RELEASE(compilationMsgs);
				SAFE_RELEASE(compiledVertexShader);
				SAFE_RELEASE(compiledPixelShader);
				return false;
			}

			// Free up any messages from compilation if any
			SAFE_RELEASE(compilationMsgs);

			// Create vertex shader
			if (_d3d9Device->CreateVertexShader((DWORD*)(compiledVertexShader->GetBufferPointer()), &shader.vertexShader9) != D3D_OK)
			{
				lastError = { GameErrors::GameDirectX9Specific,"Could not create vertex shader from \"" + vertex + "\"." };
				SAFE_RELEASE(compiledVertexShader);
				SAFE_RELEASE(compiledPixelShader);
				return false;
			}

			// Create pixel shader
			if (_d3d9Device->CreatePixelShader((DWORD*)(compiledPixelShader->GetBufferPointer()), &shader.pixelShader9) != D3D_OK)
			{
				lastError = { GameErrors::GameDirectX9Specific,"Could not create pixel shader from \"" + fragment + "\"." };
				SAFE_RELEASE(compiledVertexShader);
				SAFE_RELEASE(compiledPixelShader);
				SAFE_RELEASE(shader.vertexShader9);
				return false;
			}

			// Shaders created, release the compiled code
			SAFE_RELEASE(compiledVertexShader);
			SAFE_RELEASE(compiledPixelShader);

			return true;
		}
		else
		{
			// Load compiled vertex shader
			std::ifstream file;
			uint32_t fileSize = 0;
			uint8_t* compiledVertexShader = nullptr;
			uint8_t* compiledPixelShader = nullptr;

			file.open(vertex, std::fstream::in | std::fstream::binary | std::fstream::ate);
			if (file.is_open())
			{
				fileSize = (uint32_t)file.tellg();
				file.seekg(0, file.beg);
				compiledVertexShader = new uint8_t[fileSize];
				file.read((char *)compiledVertexShader, fileSize);
				if (!file.good())
				{
					lastError = { GameErrors::GameDirectX9Specific,"Error reading vertex shader file \"" + vertex + "\".\n" };
					if (compiledVertexShader)
					{
						delete[] compiledVertexShader;
						compiledVertexShader = nullptr;
					}
					file.close();
					return false;
				}
				file.close();

				// Create vertex shader
				if (_d3d9Device->CreateVertexShader((DWORD*)(compiledVertexShader), &shader.vertexShader9) != D3D_OK)
				{
					lastError = { GameErrors::GameDirectX9Specific,"Could not create vertex shader from \"" + vertex + "\"." };
					if (compiledVertexShader)
					{
						delete[] compiledVertexShader;
						compiledVertexShader = nullptr;
					}
					return false;
				}				
			}
			else
			{
				lastError = { GameErrors::GameDirectX9Specific, "Compiled vertex shader \"" + vertex + "\" does not exsist." };
				if (compiledVertexShader)
				{
					delete[] compiledVertexShader;
					compiledVertexShader = nullptr;
				}
				return false;
			}

			// Reset file size
			fileSize = 0;

			// Load compiled pixel shader
			file.open(fragment, std::fstream::in | std::fstream::binary | std::fstream::ate);
			if (file.is_open())
			{
				fileSize = (uint32_t)file.tellg();
				file.seekg(0, file.beg);
				compiledPixelShader = new uint8_t[fileSize];
				file.read((char*)compiledPixelShader, fileSize);
				if (!file.good())
				{
					lastError = { GameErrors::GameDirectX9Specific,"Error reading pixel shader file \"" + fragment + "\"\n" };
					if (compiledVertexShader)
					{
						delete[] compiledVertexShader;
						compiledVertexShader = nullptr;
					}
					if (compiledPixelShader)
					{
						delete[] compiledPixelShader;
						compiledPixelShader = nullptr;
					}
					SAFE_RELEASE(shader.vertexShader9);
					file.close();

					return false;
				}
			
				file.close();

				// Create pixel shader
				if (_d3d9Device->CreatePixelShader((DWORD*)(compiledPixelShader), &shader.pixelShader9) != D3D_OK)
				{
					lastError = { GameErrors::GameDirectX9Specific,"Could not create pixel shader from \"" + fragment + "\"." };
					if (compiledVertexShader)
					{
						delete[] compiledVertexShader;
						compiledVertexShader = nullptr;
					}
					if (compiledPixelShader)
					{
						delete[] compiledPixelShader;
						compiledPixelShader = nullptr;
					}
					SAFE_RELEASE(shader.vertexShader9);
					return false;
				}
			}
			else
			{
				lastError = { GameErrors::GameDirectX9Specific, "Compiled pixel shader \"" + fragment + "\" does not exsist." };
				if (compiledVertexShader)
				{
					delete[] compiledVertexShader;
					compiledVertexShader = nullptr;
				}
				if (compiledPixelShader)
				{
					delete[] compiledPixelShader;
					compiledPixelShader = nullptr;
				}
				SAFE_RELEASE(shader.vertexShader9);
				return false;
			}

			// Shaders created, release the compiled code
			if (compiledVertexShader)
			{
				delete[] compiledVertexShader;
				compiledVertexShader = nullptr;
			}
			if (compiledPixelShader)
			{
				delete[] compiledPixelShader;
				compiledPixelShader = nullptr;
			}

			return true;
		}


	}

	inline void RendererDX9::UnLoadShader(Shader& shader)
	{
		SAFE_RELEASE(shader.vertexShader9);
		SAFE_RELEASE(shader.pixelShader9);
	}
}

#endif