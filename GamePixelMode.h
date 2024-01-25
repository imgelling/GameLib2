#if !defined(GAMEPIXELMODE_H)
#define GAMEPIXELMODE_H

#if defined(GAME_OPENGL)
#include <GL/gl.h>
#endif
#if defined(GAME_DIRECTX9)
#include <d3d9.h>
#endif
#if defined(GAME_DIRECTX12)
#include <d3d12.h>
#include <initguid.h>
#include "d3dx12.h"
#endif
#include "GameErrors.h"
#include "GameEngine.h"
#include "GameMath.h"
#include "GameTexture2D.h"
#include "GameTextShaders.h"
#include "GameThreadPool.h"

// Just a test to make sure defines are working
#if defined(GAME_ENABLE_SHADERS)
#define USING_SHADERS
#endif

namespace game
{
	extern GameError lastError;
	extern Engine* enginePointer;
	class PixelMode
	{
	public:
		PixelMode();
		~PixelMode();

		bool Initialize(const Vector2i& sizeOfScreen);
		void Render();
		void Clear(const Color &color) noexcept;
		void Pixel(const int32_t x, const int32_t y, const game::Color& color) noexcept;
		void PixelClip(const int32_t x, const int32_t y, const game::Color& color) noexcept;
		void Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color& color) noexcept;
		void LineClip(const int32_t x1, const int32_t y1, const int32_t x2, const int32_t y2, const Color& color) noexcept;
		void HLine(const int32_t x1, const int32_t x2, const int32_t y, const Color& color) noexcept;
		void HLineClip(const int32_t x1, const int32_t x2, const int32_t y, const Color& color) noexcept;
		void VLine(const int32_t x, const int32_t y1, const int32_t y2, const Color& color) noexcept;
		void VLineClip(const int32_t x, const int32_t y1, const int32_t y2, const Color& color) noexcept;
		void Circle(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept;
		void CircleClip(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept;
		void CircleFilled(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept;
		void CircleFilledClip(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept;
		void Rect(const Recti& rectangle, const Color& color) noexcept;
		void RectClip(const Recti& rectangle, const Color& color) noexcept;
		void RectFilled(const Recti& rectangle, const Color& color) noexcept;
		void RectFilledClip(const Recti& rectangle, const Color& color) noexcept;
		void HPillClip(const int32_t x, const int32_t y, const int32_t length, const int32_t radius, const game::Color& color) noexcept;
		void VPillClip(const int32_t x, const int32_t y, const int32_t height, const int32_t radius, const game::Color& color) noexcept;
		void Text(const std::string& text, const int32_t x, const int32_t y, const game::Color& color, const uint32_t scale = 1);
		void TextClip(const std::string& text, const int32_t x, const int32_t y, const game::Color& color, const uint32_t scale = 1);
		Pointi GetScaledMousePosition() const noexcept;
		Pointi GetPixelFrameBufferSize() const noexcept;
		uint32_t* videoBuffer;
	private:
		ThreadPool _threadPool;
		//uint32_t* videoBuffer0;
		//uint32_t _currentVideoBuffer;
		uint32_t* videoBuffers[10];
		uint32_t numbuffers;
		Texture2D _frameBuffer;
		Vector2f _oneOverScale;
		Vector2f _savedPositionOfScaledTexture;
		Vector2i _bufferSize;
		uint32_t _totalBufferSize;
		Vector2i _windowSize;
		uint8_t* _fontROM;
		uint32_t _currentBuffer;
		void _UpdateFrameBuffer();
		void _ScaleQuadToWindow();
#if defined(GAME_OPENGL) & !defined(GAME_ENABLE_SHADERS)
		uint32_t _compiledQuad;
#endif
#if defined(GAME_DIRECTX9)
		struct _vertex9
		{
			float_t x, y, z, rhw;    
			uint32_t color;    
			float_t u, v;
		};
		_vertex9 _quadVertices9[6] =
		{
			{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255,255, 255, 255), 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255,255, 255, 255), 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255,255, 255, 255), 0.0f, 1.0f},

			{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255,255, 255, 255), 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255,255, 255, 255), 1.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255,255, 255, 255), 0.0f, 1.0f}
		};
		LPDIRECT3DVERTEXBUFFER9 _vertexBuffer9;
#endif
#if defined(GAME_DIRECTX10)
		struct _vertex10
		{
			float_t x, y, z;
			float_t r, g, b, a;
			float_t u, v;
			// D3DXColor is just a float for rgba
			// D3DXColor color
		};
		_vertex10 _quadVertices10[4] =
		{
			// tl
			{0.0f, 1.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
			// tr
			{0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, .0f},
			// bl
			{-0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},
			// br
			{0.5f, 0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		};
		Microsoft::WRL::ComPtr<ID3D10Buffer> _vertexBuffer10;
		Microsoft::WRL::ComPtr<ID3D10Buffer> _indexBuffer10;
		Shader _pixelModeShader10;
		Microsoft::WRL::ComPtr<ID3D10InputLayout> _vertexLayout10;
		Microsoft::WRL::ComPtr<ID3D10ShaderResourceView> _textureShaderResourceView_10;
		Microsoft::WRL::ComPtr<ID3D10SamplerState> _textureSamplerState10;
#endif
#if defined(GAME_DIRECTX11)
		struct _vertex11
		{
			float_t x, y, z;
			float_t r, g, b, a;
			float_t u, v;
		};
		_vertex11 _quadVertices11[4] =
		{
			// tl
			{0.0f, 1.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
			// tr
			{0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, .0f},
			// bl
			{-0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},
			// br
			{0.5f, 0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer11;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _indexBuffer11;
		Shader _pixelModeShader11;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> _vertexLayout11;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureShaderResourceView0_11;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _textureSamplerState11;
#endif
#if defined(GAME_DIRECTX12)
		struct _vertex12
		{
			float_t x, y, z;
			float_t r, g, b, a;
			float_t u, v;
		};
		_vertex12 _quadVertices12[4] =
		{
			// tl
			{0.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
			// tr
			{0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
			// bl
			{-0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},
			// br
			{0.5f, 0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		};
		Shader _pixelModeShader12;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineStateObject; 
		Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature; 
		Microsoft::WRL::ComPtr<ID3D12Resource> _indexBufferHeap; 
		D3D12_INDEX_BUFFER_VIEW _indexBufferView; 
		Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBufferHeap; 
		Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBufferUploadHeap;
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView; 
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _bundleAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _renderBundle;
#endif
	};

	inline PixelMode::PixelMode()
	{
		videoBuffer = nullptr;
		_totalBufferSize = 0;
		_fontROM = nullptr;
		_threadPool.Start();
		numbuffers = 5;
		_currentBuffer = 0;
		for (uint32_t i = 0; i < 10; i++)
			videoBuffers[i] = nullptr;
#if defined(GAME_OPENGL) & !defined(GAME_ENABLE_SHADERS)
		_compiledQuad = 0;
#endif
#if defined(GAME_DIRECTX9)
		_vertexBuffer9 = nullptr;
#endif
#if defined(GAME_DIRECTX10)
#endif
#if defined(GAME_DIRECTX11)
#endif
#if defined(GAME_DIRECTX12)
		_vertexBufferView = {};
		_indexBufferView = {};
#endif
	}

	inline PixelMode::~PixelMode()
	{
		//if (videoBuffer != nullptr) delete[] videoBuffer;
		_threadPool.Stop();
		if (_fontROM != nullptr) delete[] _fontROM;
		for (uint32_t i = 0; i < numbuffers; i++)
			if (videoBuffers[i]) delete[] videoBuffers[i];
#if defined (GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			glDeleteLists(_compiledQuad, 1);
		}
#endif
#if defined (GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			if (_vertexBuffer9)
			{
				_vertexBuffer9->Release();
				_vertexBuffer9 = nullptr;
			}
		}
#endif
#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			enginePointer->geUnLoadShader(_pixelModeShader10);
		}
#endif
#if defined(GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			enginePointer->geUnLoadShader(_pixelModeShader11);
		}
#endif
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			enginePointer->geUnLoadShader(_pixelModeShader12);
		}
#endif
		enginePointer->geUnLoadTexture(_frameBuffer);
	}

	inline bool PixelMode::Initialize(const Vector2i& sizeOfScreen)
	{
		_bufferSize = sizeOfScreen;
		_totalBufferSize = sizeOfScreen.width * sizeOfScreen.height;
		// Save window size
		_windowSize = enginePointer->geGetWindowSize();

		// Create video buffers
		videoBuffer = new uint32_t[_totalBufferSize];
		if (videoBuffer == nullptr)
		{
			lastError = { GameErrors::GameRenderer, "Could not allocate RAM for PixelMode video buffer 0." };
			return false;
		}
		//Clear(Colors::Black);
		std::fill_n(videoBuffer, _totalBufferSize, Colors::Black.packedABGR); //1075
		for (uint32_t i = 0; i < numbuffers; i++)
		{
			videoBuffers[i] = new uint32_t[_totalBufferSize];
			memcpy(videoBuffers[i], videoBuffer, (size_t)_totalBufferSize * 4);
		}
		delete [] videoBuffer;
		videoBuffer = videoBuffers[_currentBuffer];

		// Create frame buffer texture

		_frameBuffer.width = _bufferSize.width;
		_frameBuffer.height = _bufferSize.height;
		_frameBuffer.componentsPerPixel = 4;
		_frameBuffer.filterType = game::TextureFilterType::Point;
		_frameBuffer.isMipMapped = false;
		_frameBuffer.name = "PixelMode FrameBuffer";
		if (!enginePointer->geCreateTexture(_frameBuffer))
		{
			lastError = { GameErrors::GameRenderer, "Could not create textures for PixelMode frame buffers." };
			return false;
		}

		// Create the font
		_fontROM = new uint8_t[128 * 48];
		ZeroMemory(_fontROM, (size_t)128 * (size_t)48);
		std::string data;
		data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
		data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
		data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
		data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
		data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
		data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
		data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
		data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
		data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
		data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
		data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
		data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
		data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
		data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
		data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
		data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

		int px = 0, py = 0;
		for (size_t b = 0; b < 1024; b += 4)
		{
			uint32_t sym1 = (uint32_t)data[b + 0] - 48;
			uint32_t sym2 = (uint32_t)data[b + 1] - 48;
			uint32_t sym3 = (uint32_t)data[b + 2] - 48;
			uint32_t sym4 = (uint32_t)data[b + 3] - 48;
			uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

			for (int i = 0; i < 24; i++)
			{
				uint32_t k = r & (1 << i) ? 255 : 0;
				_fontROM[py * 128 + px] = k;
				if (++py == 48) { px++; py = 0; }
			}
		}


#if defined(GAME_OPENGL) & !defined(GAME_ENABLE_SHADERS)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			_compiledQuad = glGenLists(1);
		}
#endif
#if defined (GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			enginePointer->d3d9Device->CreateVertexBuffer(6 * sizeof(_vertex9), 0, (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1), D3DPOOL_MANAGED, &_vertexBuffer9, NULL);
			if (_vertexBuffer9 == nullptr)
			{
				lastError = { GameErrors::GameDirectX9Specific, "Could not create vertex buffer for PixelMode." };
				return false;
			}
		}
#endif
#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			D3D10_BUFFER_DESC vertexBufferDescription = { 0 };
			D3D10_BUFFER_DESC indexBufferDescription = { 0 };
			D3D10_SUBRESOURCE_DATA vertexInitialData = { 0 };
			D3D10_SUBRESOURCE_DATA indexInitialData = { 0 };
			DWORD indices[] = { 0, 1, 2, 1, 3, 2, };
			D3D10_INPUT_ELEMENT_DESC inputLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0},
			};
			
			// Load shaders for sprite mode
			//if (!enginePointer->geLoadShader("Content/VertexShader.hlsl", "Content/PixelShader.hlsl", _pixelModeShader10))
			if (!enginePointer->geLoadTextShader(gameTextShaders, "mainVS", "mainPS", _pixelModeShader10))
			{
				return false;
			}
			
			// Create the vertex buffer
			vertexBufferDescription.ByteWidth = sizeof(_vertex10) * 4;
			vertexBufferDescription.Usage = D3D10_USAGE_DYNAMIC;// D3D10_USAGE_DEFAULT;
			vertexBufferDescription.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			vertexBufferDescription.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE; //0;
			vertexBufferDescription.MiscFlags = 0;
			vertexInitialData.pSysMem = _quadVertices10;
			if (FAILED(enginePointer->d3d10Device->CreateBuffer(&vertexBufferDescription, &vertexInitialData, _vertexBuffer10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create vertex buffer for PixelMode." };
				enginePointer->geUnLoadShader(_pixelModeShader10);
				return false;
			}

			// Create index buffer
			indexBufferDescription.Usage = D3D10_USAGE_IMMUTABLE;
			indexBufferDescription.ByteWidth = sizeof(DWORD) * 2 * 3;
			indexBufferDescription.BindFlags = D3D10_BIND_INDEX_BUFFER;
			indexBufferDescription.CPUAccessFlags = 0;
			indexBufferDescription.MiscFlags = 0;
			indexInitialData.pSysMem = indices;
			if (FAILED(enginePointer->d3d10Device->CreateBuffer(&indexBufferDescription, &indexInitialData, _indexBuffer10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create index buffer for PixelMode." };
				enginePointer->geUnLoadShader(_pixelModeShader10);
				return false;
			}

			// Create input layout for shaders
			if (FAILED(enginePointer->d3d10Device->CreateInputLayout(inputLayout, 3, _pixelModeShader10.compiledVertexShader10->GetBufferPointer(), _pixelModeShader10.compiledVertexShader10->GetBufferSize(), _vertexLayout10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create input layout for PixelMode." };
				enginePointer->geUnLoadShader(_pixelModeShader10);
				return false;
			}

			D3D10_SAMPLER_DESC samplerDesc = { };
			//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
			samplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
			samplerDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D10_FLOAT32_MAX;

			if (FAILED(enginePointer->d3d10Device->CreateSamplerState(&samplerDesc, _textureSamplerState10.GetAddressOf())))
			{
				std::cout << "Create sampler failed!\n";
			}

			D3D10_SHADER_RESOURCE_VIEW_DESC srDesc = {};
			srDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = 1;
			if (FAILED(enginePointer->d3d10Device->CreateShaderResourceView(_frameBuffer.textureInterface10.Get(), &srDesc, _textureShaderResourceView_10.GetAddressOf())))
			{
				std::cout << "CreateSRV0 failed!\n";
			}
		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			D3D11_SUBRESOURCE_DATA vertexInitialData = { 0 };
			D3D11_SUBRESOURCE_DATA indexInitialData = { 0 };
			DWORD indices[] = { 0, 1, 2, 1, 3, 2, };
			D3D11_INPUT_ELEMENT_DESC inputLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			// Load shaders for sprite mode
			//if (!enginePointer->geLoadShader("Content/VertexShader.hlsl", "Content/PixelShader.hlsl", _pixelModeShader11))
			if (!enginePointer->geLoadTextShader(gameTextShaders, "mainVS", "mainPS", _pixelModeShader11))
			{
				return false;
			}

			// Create the vertex buffer
			vertexBufferDescription.ByteWidth = sizeof(_vertex11) * 4;
			vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;// D3D10_USAGE_DEFAULT;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //0;
			vertexBufferDescription.MiscFlags = 0;
			vertexInitialData.pSysMem = _quadVertices11;
			if (FAILED(enginePointer->d3d11Device->CreateBuffer(&vertexBufferDescription, &vertexInitialData, _vertexBuffer11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create vertex buffer for PixelMode." };
				enginePointer->geUnLoadShader(_pixelModeShader11);
				return false;
			}

			// Create index buffer
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.ByteWidth = sizeof(DWORD) * 2 * 3;
			indexBufferDescription.BindFlags = D3D10_BIND_INDEX_BUFFER;
			indexBufferDescription.CPUAccessFlags = 0;
			indexBufferDescription.MiscFlags = 0;
			indexInitialData.pSysMem = indices;
			if (FAILED(enginePointer->d3d11Device->CreateBuffer(&indexBufferDescription, &indexInitialData, _indexBuffer11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create index buffer for PixelMode." };
				enginePointer->geUnLoadShader(_pixelModeShader11);
				return false;
			}

			// Create input layout for shaders
			if (FAILED(enginePointer->d3d11Device->CreateInputLayout(inputLayout, 3, _pixelModeShader11.compiledVertexShader11->GetBufferPointer(), _pixelModeShader11.compiledVertexShader11->GetBufferSize(), _vertexLayout11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create input layout for PixelMode." };
				enginePointer->geUnLoadShader(_pixelModeShader11);
				return false;
			}

			D3D11_SAMPLER_DESC samplerDesc = { };
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			if (FAILED(enginePointer->d3d11Device->CreateSamplerState(&samplerDesc, _textureSamplerState11.GetAddressOf())))
			{
				std::cout << "Create sampler failed!\n";
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc = {};
			srDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = 1;
			if (FAILED(enginePointer->d3d11Device->CreateShaderResourceView(_frameBuffer.textureInterface11.Get(), &srDesc, _textureShaderResourceView0_11.GetAddressOf())))
			{
				std::cout << "CreateSRV failed!\n";
			}
		}
#endif
#if defined (GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			// Root signature description
			HRESULT hr = {};
			// Create the root signature.

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

			// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(enginePointer->d3d12Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			CD3DX12_DESCRIPTOR_RANGE1 ranges[1] = {};
			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

			CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.MipLODBias = 0;
			sampler.MaxAnisotropy = 0;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

			Microsoft::WRL::ComPtr<ID3DBlob> signature;
			hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not serialize root signature in pixlemode." };
				AppendHR12(hr);
				return false;
			}

			// create the root signature
			hr = enginePointer->d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific, "could not create root signature in pixelmode." };
				AppendHR12(hr);
				return false;
			}
			_rootSignature->SetName(L"pixelmode root signature");


			// Load shaders for sprite mode
			//if (!enginePointer->geLoadShader("Content/VertexShader.hlsl", "Content/PixelShader.hlsl", _pixelModeShader12))
			if (!enginePointer->geLoadTextShader(gameTextShaders, "mainVS", "mainPS", _pixelModeShader12))
			{
				return false;
			}

			// Input layout
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};

			// Describe input layout
			D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
			inputLayoutDesc.NumElements = ARRAYSIZE(inputElementDescs);
			inputLayoutDesc.pInputElementDescs = inputElementDescs;

			
			// Describe PSO
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; 
			psoDesc.InputLayout = inputLayoutDesc; 
			psoDesc.pRootSignature = _rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(_pixelModeShader12.compiledVertexShader12.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(_pixelModeShader12.compiledPixelShader12.Get());
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; 
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc = { 1, 0 };
			psoDesc.SampleMask = 0xffffffff; 
			D3D12_RASTERIZER_DESC rasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			//rasterDesc.FrontCounterClockwise = FALSE;
			psoDesc.RasterizerState = rasterDesc;
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
			{
				TRUE,FALSE,
				D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
				D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
				D3D12_LOGIC_OP_NOOP,
				D3D12_COLOR_WRITE_ENABLE_ALL,
			};
			psoDesc.BlendState.RenderTarget[0] = defaultRenderTargetBlendDesc;
			psoDesc.NumRenderTargets = 1;
			psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

			// Create the pso
			hr = enginePointer->d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineStateObject));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create pipline state for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_pipelineStateObject->SetName(L"PixelMode PSO");

			// Create vertex buffer
			uint32_t vBufferSize = sizeof(_quadVertices12);

			// Create vertex buffer heap
			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc,	D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_vertexBufferHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create vertex buffer heap for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_vertexBufferHeap->SetName(L"PixelMode Vertex Buffer Heap");

			// Create vertex buffer upload heap
			heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			resDesc = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp,	D3D12_HEAP_FLAG_NONE, &resDesc,	D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,	IID_PPV_ARGS(&_vertexBufferUploadHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create vertex buffer upload heap for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_vertexBufferUploadHeap->SetName(L"PixelMode Vertex Buffer Upload Heap");


			// Index buffer
			DWORD iList[] = {
				0, 1, 2, 
				1, 3, 2
			};
			uint32_t iBufferSize = sizeof(iList);

			// Create index buffer heap
			heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			resDesc = CD3DX12_RESOURCE_DESC::Buffer(iBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_indexBufferHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create index buffer upload heap for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_indexBufferHeap->SetName(L"PixelMode Index Buffer Heap");

			// Create a temporary upload heap to upload index buffer
			Microsoft::WRL::ComPtr<ID3D12Resource> _indexBufferUploadHeap;
			heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			resDesc = CD3DX12_RESOURCE_DESC::Buffer(iBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp,	D3D12_HEAP_FLAG_NONE, &resDesc,	D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_indexBufferUploadHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create vertex buffer upload heap for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_indexBufferUploadHeap->SetName(L"PixelMode Index Buffer Upload Heap");

			RendererDX12* temp = enginePointer->geGetRenderer();


			// resets the command list -----------------------------
			{
				// once per frame
				//if (FAILED(temp->_commandAllocator[temp->_frameIndex]->Reset()))
				//{
				//	std::cout << "Command allocator reset failed\n";
				//}

				// Reset to start recording
				if (FAILED(enginePointer->commandList->Reset(temp->_commandAllocator[temp->_frameIndex].Get(), NULL)))
				{
					std::cout << "command list reset failed\n";
				}
			}
			// end of command list reset ---------------------------------------
			
			// Fill out resource data with vertex data
			D3D12_SUBRESOURCE_DATA vertexData = {};
			vertexData.pData = reinterpret_cast<uint8_t*>(_quadVertices12);
			vertexData.RowPitch = vBufferSize;
			vertexData.SlicePitch = 0;

			// Create a command to copy vertex buffer
			UpdateSubresources(enginePointer->commandList.Get(), _vertexBufferHeap.Get(), _vertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

			// Transition the vertex buffer data from copy destination state to vertex buffer state
			CD3DX12_RESOURCE_BARRIER resBar = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBufferHeap.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			enginePointer->commandList->ResourceBarrier(1, &resBar);

			// Fill out resource data with index buffer
			D3D12_SUBRESOURCE_DATA indexData = {};
			indexData.pData = reinterpret_cast<uint8_t*>(iList);
			indexData.RowPitch = iBufferSize;
			indexData.SlicePitch = 0;

			// Create a command to copy index buffer
			UpdateSubresources(enginePointer->commandList.Get(), _indexBufferHeap.Get(), _indexBufferUploadHeap.Get(), 0, 0, 1, &indexData);

			// Transition the index buffer data from copy destination state to vertex buffer state
			CD3DX12_RESOURCE_BARRIER ibufferbar = CD3DX12_RESOURCE_BARRIER::Transition(_indexBufferHeap.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			enginePointer->commandList->ResourceBarrier(1, &ibufferbar);

			// Execute the command list to upload the initial data
			{
				enginePointer->commandList->Close();
				ID3D12CommandList* ppCommandLists[] = { enginePointer->commandList.Get() };
				enginePointer->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

				// ??
				// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
				temp->_fenceValue[temp->_frameIndex]++;
				hr = enginePointer->commandQueue->Signal(temp->_fence[temp->_frameIndex].Get(), temp->_fenceValue[temp->_frameIndex]);
				if (FAILED(hr))
				{
					lastError = { GameErrors::GameDirectX12Specific,"Pixel mode signal failed." };
					AppendHR12(hr);
					return false;
				}
			}

			//temp->_WaitForPreviousFrame(false);

			// Create a vertex buffer view for the quad
			_vertexBufferView.BufferLocation = _vertexBufferHeap->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes = sizeof(_vertex12);
			_vertexBufferView.SizeInBytes = vBufferSize;

			// Create a index buffer view for the quad
			_indexBufferView.BufferLocation = _indexBufferHeap->GetGPUVirtualAddress();
			_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			_indexBufferView.SizeInBytes = iBufferSize;

			// Create render bundle now the index and vertex buffers have been made
			if (FAILED(enginePointer->d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&_bundleAllocator))))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create bundle allocator for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_bundleAllocator->SetName(L"PixelMode Bundle Allocator");
			if (FAILED(enginePointer->d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, _bundleAllocator.Get(), _pipelineStateObject.Get(), IID_PPV_ARGS(&_renderBundle))))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create bundle command list for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			_renderBundle->SetName(L"PixelMode Render Bundle");

			// Describe and create a SRV for the texture.
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			// needs to be saved in texture 2d
			D3D12_RESOURCE_DESC textureDesc = {};
			textureDesc.MipLevels = 1;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.Width = _frameBuffer.width;
			textureDesc.Height = _frameBuffer.height;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			textureDesc.DepthOrArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			enginePointer->d3d12Device->CreateShaderResourceView(_frameBuffer.textureResource12.Get(), &srvDesc, _frameBuffer.srvHeap->GetCPUDescriptorHandleForHeapStart());

			// Record render bundle
			_renderBundle->SetPipelineState(_pipelineStateObject.Get()); // may not need to record
			_renderBundle->SetGraphicsRootSignature(_rootSignature.Get());
			ID3D12DescriptorHeap* ppHeaps[] = { _frameBuffer.srvHeap.Get() };
			_renderBundle->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
			_renderBundle->SetGraphicsRootDescriptorTable(0, _frameBuffer.srvHeap->GetGPUDescriptorHandleForHeapStart());
			_renderBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_renderBundle->IASetVertexBuffers(0, 1, &_vertexBufferView);
			_renderBundle->IASetIndexBuffer(&_indexBufferView);
			_renderBundle->DrawIndexedInstanced(6, 1, 0, 0, 0);
			if (FAILED(_renderBundle->Close()))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Closing render bundle failed for PixelMode." };
				AppendHR12(hr);
				return false;
			}
			enginePointer->geGetRenderer()->flushGPU();
		}
#endif

		// Scale the texture to window size
		//_ScaleQuadToWindow();
		return true;
	}

	inline void PixelMode::_UpdateFrameBuffer()
	{
#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			glBindTexture(GL_TEXTURE_2D, _frameBuffer.bind);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _frameBuffer.width, _frameBuffer.height, GL_RGBA, game::systemInfo.gpuInfo.internalPixelType, (GLvoid*)videoBuffer);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
#endif
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			D3DLOCKED_RECT rect;
			unsigned char* test = (unsigned char*)videoBuffer;
			_frameBuffer.textureInterface9->LockRect(0, &rect, 0, 0);
			unsigned char* dest = static_cast<unsigned char*>(rect.pBits);
			memcpy(dest, test, sizeof(unsigned char) * _frameBuffer.width * _frameBuffer.height * 4);
			_frameBuffer.textureInterface9->UnlockRect(0);
		}
#endif
#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			D3D10_MAPPED_TEXTURE2D mappedTex = { 0 };
			if (FAILED(_frameBuffer.textureInterface10->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex)))
			{
				std::cout << "Could not map framebuffer in PixelMode.\n";
				return;
			}
			unsigned char* dest = (unsigned char*)mappedTex.pData;
			memcpy(dest, (unsigned char*)videoBuffer, sizeof(unsigned char) * _frameBuffer.width * _frameBuffer.height * 4);

			_frameBuffer.textureInterface10->Unmap(D3D10CalcSubresource(0, 0, 1));
		}
#endif
#if defined(GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			D3D11_MAPPED_SUBRESOURCE data;
			if (FAILED(enginePointer->d3d11DeviceContext->Map(_frameBuffer.textureInterface11.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
			{
				std::cout << "Could not map framebuffer in PixelMode.\n.";
			}
			memcpy(data.pData, (unsigned char*)videoBuffer, sizeof(unsigned char) * _frameBuffer.width * _frameBuffer.height * 4);
			enginePointer->d3d11DeviceContext->Unmap(_frameBuffer.textureInterface11.Get(), 0);
		}
#endif
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData = reinterpret_cast<uint8_t*>(videoBuffer);
			textureData.RowPitch = static_cast<LONG_PTR>(_frameBuffer.width) * 4;
			textureData.SlicePitch = 0;// textureData.RowPitch* _frameBuffer[_currentBuffer].height;
			CD3DX12_RESOURCE_BARRIER trans = CD3DX12_RESOURCE_BARRIER::Transition(_frameBuffer.textureResource12.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
			enginePointer->commandList->ResourceBarrier(1, &trans);
			UpdateSubresources(enginePointer->commandList.Get(), _frameBuffer.textureResource12.Get(), _frameBuffer.textureUploadHeap12.Get(), 0, 0, 1, &textureData);
			trans = CD3DX12_RESOURCE_BARRIER::Transition(_frameBuffer.textureResource12.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			enginePointer->commandList->ResourceBarrier(1, &trans);
		}
#endif
	}

	inline void PixelMode::_ScaleQuadToWindow()
	{
		float_t tempScale = 0.0f;
		Vector2f _scale;
		Vector2f _sizeOfScaledTexture;
		Vector2f _positionOfScaledTexture;

		if (_windowSize.height < _windowSize.width)
		{
			_scale.y = (float_t)_windowSize.height * _frameBuffer.oneOverHeight;
			tempScale = (float_t)_windowSize.width * _frameBuffer.oneOverWidth;
			if (tempScale > _scale.y)
			{
				_scale.x = _scale.y;
			}
			else
			{
				_scale.x = _scale.y = tempScale;
				_positionOfScaledTexture.y = ((_windowSize.height >> 1) - ((float_t)_frameBuffer.height * _scale.y / 2.0f));
			}
			_positionOfScaledTexture.x = ((_windowSize.width >> 1) - ((float_t)_frameBuffer.width * _scale.x / 2.0f));
		}
		else if (_windowSize.height > _windowSize.width)
		{
			_scale.x = (float_t)_windowSize.width * _frameBuffer.oneOverWidth;
			_scale.y = _scale.x;
			_positionOfScaledTexture.y = ((_windowSize.height >> 1) - ((float_t)_frameBuffer.height * _scale.y / 2.0f));
		}
		else
		{
			_scale = { 1.0f, 1.0f };
		}

		_oneOverScale.x = 1.0f / _scale.x;
		_oneOverScale.y = 1.0f / _scale.y;

		// Set the size of the scaled texture
		_sizeOfScaledTexture.width = _positionOfScaledTexture.x + (_frameBuffer.width * _scale.x);
		_sizeOfScaledTexture.height = _positionOfScaledTexture.y + (_frameBuffer.height * _scale.y);

		// Pixel offset fix (may be wrecking dx10 and 11)
#if defined(GAME_DIRECTX9)  ||  defined(GAME_OPENGL)
		//if (enginePointer->geIsUsing(GAME_DIRECTX9) || enginePointer->geIsUsing(GAME_OPENGL))
		{
			_positionOfScaledTexture.x -= _frameBuffer.oneOverWidth;
			_positionOfScaledTexture.y -= _frameBuffer.oneOverHeight;
			_sizeOfScaledTexture.width -= _frameBuffer.oneOverWidth;
			_sizeOfScaledTexture.height -= _frameBuffer.oneOverHeight;
		}
#endif

		_savedPositionOfScaledTexture = _positionOfScaledTexture;

#if defined(GAME_OPENGL) & !defined(GAME_ENABLE_SHADERS)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{

			// Homoginize the scaled rect to -1 to 1 range using
			_positionOfScaledTexture.x = (_positionOfScaledTexture.x * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_positionOfScaledTexture.y = 1.0f - (_positionOfScaledTexture.y * 2.0f / (float_t)_windowSize.height);// -1.0f;
			_sizeOfScaledTexture.width = ((float_t)_sizeOfScaledTexture.width * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_sizeOfScaledTexture.height = 1.0f - ((float_t)_sizeOfScaledTexture.height * 2.0f / (float_t)_windowSize.height);// -1.0f;

			glDeleteLists(_compiledQuad,1);
			glNewList(_compiledQuad, GL_COMPILE);
			{
				glBegin(GL_QUADS);
				//bl
				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2f(0, 1);
				glVertex2f(_positionOfScaledTexture.x, _sizeOfScaledTexture.height);
				//br
				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2f(1, 1);
				glVertex2f(_sizeOfScaledTexture.width, _sizeOfScaledTexture.height);
				//tr
				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(_sizeOfScaledTexture.width, _positionOfScaledTexture.y);
				// tl
				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2f(0, 0);
				glVertex2f(_positionOfScaledTexture.x, _positionOfScaledTexture.y);

				glEnd();
			}
			glEndList();
		}
#endif
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			VOID* pVoid = nullptr;  

			// tl
			_quadVertices9[0].x = _positionOfScaledTexture.x;
			_quadVertices9[0].y = _positionOfScaledTexture.y;
			// tr
			_quadVertices9[1].x = _sizeOfScaledTexture.width;
			_quadVertices9[1].y = _positionOfScaledTexture.y;
			// bl
			_quadVertices9[2].x = _positionOfScaledTexture.x;
			_quadVertices9[2].y = _sizeOfScaledTexture.height;

			// tr
			_quadVertices9[3].x = _sizeOfScaledTexture.width;
			_quadVertices9[3].y = _positionOfScaledTexture.y;
			// br
			_quadVertices9[4].x = _sizeOfScaledTexture.width;
			_quadVertices9[4].y = _sizeOfScaledTexture.height;
			// bl
			_quadVertices9[5].x = _positionOfScaledTexture.x;
			_quadVertices9[5].y = _sizeOfScaledTexture.height;

			// Copy vertices to the vertex buffer
			_vertexBuffer9->Lock(0, 0, (void**)&pVoid, 0);
			memcpy(pVoid, _quadVertices9, sizeof(_quadVertices9));
			_vertexBuffer9->Unlock();
		}
#endif
#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			// Homoginize the scaled rect to -1 to 1 range using
			_positionOfScaledTexture.x = (_positionOfScaledTexture.x * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_positionOfScaledTexture.y = 1.0f - (_positionOfScaledTexture.y * 2.0f / (float_t)_windowSize.height);
			_sizeOfScaledTexture.width = ((float_t)_sizeOfScaledTexture.width * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_sizeOfScaledTexture.height = 1.0f - ((float_t)_sizeOfScaledTexture.height * 2.0f / (float_t)_windowSize.height);

			// tl
			_quadVertices10[0].x = _positionOfScaledTexture.x;
			_quadVertices10[0].y = _positionOfScaledTexture.y;
			// tr
			_quadVertices10[1].x = _sizeOfScaledTexture.width;
			_quadVertices10[1].y = _positionOfScaledTexture.y;
			// bl
			_quadVertices10[2].x = _positionOfScaledTexture.x;
			_quadVertices10[2].y = _sizeOfScaledTexture.height;

			// br
			_quadVertices10[3].x = _sizeOfScaledTexture.width;
			_quadVertices10[3].y = _sizeOfScaledTexture.height;

			VOID* pVoid = nullptr;
			_vertexBuffer10->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);
			memcpy(pVoid, _quadVertices10, sizeof(_quadVertices10));
			_vertexBuffer10->Unmap();
		}
#endif
#if defined(GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			// Homoginize the scaled rect to -1 to 1 range using
			_positionOfScaledTexture.x = (_positionOfScaledTexture.x * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_positionOfScaledTexture.y = 1.0f - (_positionOfScaledTexture.y * 2.0f / (float_t)_windowSize.height);// -1.0f;
			_sizeOfScaledTexture.width = ((float_t)_sizeOfScaledTexture.width * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_sizeOfScaledTexture.height = 1.0f - ((float_t)_sizeOfScaledTexture.height * 2.0f / (float_t)_windowSize.height);// -1.0f;

			// tl
			_quadVertices11[0].x = _positionOfScaledTexture.x;
			_quadVertices11[0].y = _positionOfScaledTexture.y;
			// tr
			_quadVertices11[1].x = _sizeOfScaledTexture.width;
			_quadVertices11[1].y = _positionOfScaledTexture.y;
			// bl
			_quadVertices11[2].x = _positionOfScaledTexture.x;
			_quadVertices11[2].y = _sizeOfScaledTexture.height;
			// br
			_quadVertices11[3].x = _sizeOfScaledTexture.width;
			_quadVertices11[3].y = _sizeOfScaledTexture.height;

			D3D11_MAPPED_SUBRESOURCE data;
			if (FAILED(enginePointer->d3d11DeviceContext->Map(_vertexBuffer11.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
			{
				std::cout << "Could not map vertexbuffer in pixelmode\n.";
			}
			memcpy(data.pData, _quadVertices11, sizeof(_quadVertices11));
			enginePointer->d3d11DeviceContext->Unmap(_vertexBuffer11.Get(), 0);
		}
#endif
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			// Homoginize the scaled rect to -1 to 1 range using
			_positionOfScaledTexture.x = (_positionOfScaledTexture.x * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_positionOfScaledTexture.y = 1.0f - (_positionOfScaledTexture.y * 2.0f / (float_t)_windowSize.height);
			_sizeOfScaledTexture.width = ((float_t)_sizeOfScaledTexture.width * 2.0f / (float_t)_windowSize.width) - 1.0f;
			_sizeOfScaledTexture.height = 1.0f - ((float_t)_sizeOfScaledTexture.height * 2.0f / (float_t)_windowSize.height);
	
			// tl -1,1
			_quadVertices12[0].x = _positionOfScaledTexture.x;
			_quadVertices12[0].y = _positionOfScaledTexture.y;
			// tr 1,1
			_quadVertices12[1].x = _sizeOfScaledTexture.width;
			_quadVertices12[1].y = _positionOfScaledTexture.y;
			// bl -1,-1
			_quadVertices12[2].x = _positionOfScaledTexture.x;
			_quadVertices12[2].y = _sizeOfScaledTexture.height;

			// br
			_quadVertices12[3].x = _sizeOfScaledTexture.width;
			_quadVertices12[3].y = _sizeOfScaledTexture.height;

			int vBufferSize = sizeof(_quadVertices12);
			D3D12_SUBRESOURCE_DATA vertexData = {};
			vertexData.pData = reinterpret_cast<BYTE*>(_quadVertices12); // pointer to our vertex array
			vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
			vertexData.SlicePitch = 0; // also the size of our triangle vertex data

			// Turn vertex buffer into a destination state
			CD3DX12_RESOURCE_BARRIER resBar = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBufferHeap.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
			enginePointer->commandList->ResourceBarrier(1, &resBar);

			// Upload the vertex buffer to the vertex buffer heap
			UpdateSubresources(enginePointer->commandList.Get(), _vertexBufferHeap.Get(), _vertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

			// Turn vertex buffer into a vertex buffer state again
			CD3DX12_RESOURCE_BARRIER resBar2 = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBufferHeap.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			enginePointer->commandList->ResourceBarrier(1, &resBar2);
		}
#endif


	}

	inline void PixelMode::Render()
	{
		static bool first = true;
		Vector2i currentWindowSize = enginePointer->geGetWindowSize();

		// If window size has changed
		if ((currentWindowSize.width != _windowSize.width) || (currentWindowSize.height != _windowSize.height) || (first))
		{
			first = false;
			_windowSize = currentWindowSize;

			// Rescale the quad to fit new window size
			_ScaleQuadToWindow();
		}

		// Copy video buffer to gpu
		_UpdateFrameBuffer();

		// Draw the quad
#if defined(GAME_OPENGL) & !defined(GAME_ENABLE_SHADERS)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			glPushAttrib(GL_TEXTURE_2D);
			glEnable(GL_TEXTURE_2D);
			uint32_t boundTexture = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&boundTexture);
			glBindTexture(GL_TEXTURE_2D, _frameBuffer.bind);
			glPushAttrib(0x809D); // GL_MULTISAMPLE
			//if (enginePointer->_attributes.MultiSamples > 1)
			//{
			//	glDisable(0x809D); // 0x809D is GL_MULTISAMPLE
			//}
			glCallList(_compiledQuad);
			//if (enginePointer->_attributes.MultiSamples > 1)
			//{
			//	glEnable(0x809D);
			//}
			//glDisable(GL_TEXTURE_2D);
			if (boundTexture)
			{
				glBindTexture(GL_TEXTURE_2D, boundTexture);
			}
			glPopAttrib(); // GL_MULTISAMPLE
			glPopAttrib(); // GL_TEXTURE_2D
			
		}
#endif
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			DWORD oldFVF = 0;
			IDirect3DBaseTexture9* activeTexture = 0;
			//enginePointer->d3d9Device->BeginScene();
			// Save current state
			enginePointer->d3d9Device->GetFVF(&oldFVF);
			enginePointer->d3d9Device->GetTexture(0, &activeTexture);

			// Disable multisampling if enabled
			if (enginePointer->_attributes.MultiSamples > 1)
			{
				enginePointer->d3d9Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
			}

			enginePointer->d3d9Device->SetTexture(0, _frameBuffer.textureInterface9);
			enginePointer->d3d9Device->SetFVF((D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));
			enginePointer->d3d9Device->SetStreamSource(0, _vertexBuffer9, 0, sizeof(_vertex9));
			enginePointer->d3d9Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			// Restore previous state
			enginePointer->d3d9Device->SetFVF(oldFVF);
			enginePointer->d3d9Device->SetTexture(0, activeTexture);
			if (activeTexture)
			{
				activeTexture->Release();
			}

			// Renable multisampling if it was enabled
			if (enginePointer->_attributes.MultiSamples > 1)
			{
				enginePointer->d3d9Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
			}

			//enginePointer->d3d9Device->EndScene();
		}
		
#endif

#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			uint32_t stride = sizeof(_vertex10);
			uint32_t oldStride = 0;
			uint32_t offset = 0;
			uint32_t oldOffset = 0;
			Microsoft::WRL::ComPtr<ID3D10Buffer> oldVertexBuffer;
			Microsoft::WRL::ComPtr<ID3D10Buffer> oldIndexBuffer;
			DXGI_FORMAT oldIndexFormat = {};
			uint32_t oldIndexOffset = 0;
			Microsoft::WRL::ComPtr<ID3D10InputLayout> oldInputLayout = nullptr;
			Microsoft::WRL::ComPtr<ID3D10VertexShader> oldVertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D10PixelShader> oldPixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D10SamplerState> oldTextureSamplerState = nullptr;
			D3D10_PRIMITIVE_TOPOLOGY oldPrimitiveTopology = {};


			// Save everything we modify
			enginePointer->d3d10Device->IAGetIndexBuffer(oldIndexBuffer.GetAddressOf(), &oldIndexFormat, &oldIndexOffset);
			enginePointer->d3d10Device->IAGetVertexBuffers(0, 1, oldVertexBuffer.GetAddressOf(), &oldStride, &oldOffset);
			enginePointer->d3d10Device->IAGetInputLayout(oldInputLayout.GetAddressOf());
			enginePointer->d3d10Device->VSGetShader(oldVertexShader.GetAddressOf());
			enginePointer->d3d10Device->PSGetShader(oldPixelShader.GetAddressOf());
			enginePointer->d3d10Device->PSGetSamplers(0, 1, oldTextureSamplerState.GetAddressOf());
			enginePointer->d3d10Device->IAGetPrimitiveTopology(&oldPrimitiveTopology);


			// Change what we need
			enginePointer->d3d10Device->IASetIndexBuffer(_indexBuffer10.Get(), DXGI_FORMAT_R32_UINT, 0);
			enginePointer->d3d10Device->IASetVertexBuffers(0, 1, _vertexBuffer10.GetAddressOf(), &stride, &offset);
			enginePointer->d3d10Device->IASetInputLayout(_vertexLayout10.Get());
			enginePointer->d3d10Device->VSSetShader(_pixelModeShader10.vertexShader10.Get());
			enginePointer->d3d10Device->PSSetShader(_pixelModeShader10.pixelShader10.Get());
			enginePointer->d3d10Device->PSSetSamplers(0, 1, _textureSamplerState10.GetAddressOf());
			enginePointer->d3d10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			enginePointer->d3d10Device->PSSetShaderResources(0, 1, _textureShaderResourceView_10.GetAddressOf());


			enginePointer->d3d10Device->DrawIndexed(6, 0, 0);
			
			// Restore old states
			enginePointer->d3d10Device->IASetIndexBuffer(oldIndexBuffer.Get(), oldIndexFormat, oldIndexOffset);
			enginePointer->d3d10Device->IASetVertexBuffers(0, 1, oldVertexBuffer.GetAddressOf(), &oldStride, &oldOffset);
			enginePointer->d3d10Device->IASetInputLayout(oldInputLayout.Get());
			enginePointer->d3d10Device->VSSetShader(oldVertexShader.Get());
			enginePointer->d3d10Device->PSSetShader(oldPixelShader.Get());
			enginePointer->d3d10Device->PSSetSamplers(0, 1, &oldTextureSamplerState);
			if (oldPrimitiveTopology != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
			{
				enginePointer->d3d10Device->IASetPrimitiveTopology(oldPrimitiveTopology);
			}
		}
#endif
#if defined(GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			uint32_t stride = sizeof(_vertex11);
			uint32_t oldStride = 0;
			uint32_t offset = 0;
			uint32_t oldOffset = 0;
			Microsoft::WRL::ComPtr<ID3D11Buffer> oldVertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> oldIndexBuffer;
			DXGI_FORMAT oldIndexFormat = {};
			uint32_t oldIndexOffset = 0;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> oldInputLayout;
			Microsoft::WRL::ComPtr<ID3D11VertexShader> oldVertexShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> oldPixelShader;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> oldTextureSamplerState;
			D3D11_PRIMITIVE_TOPOLOGY oldPrimitiveTopology = {};


			// Save everything we modify
			enginePointer->d3d11DeviceContext->IAGetIndexBuffer(oldIndexBuffer.GetAddressOf(), &oldIndexFormat, &oldIndexOffset);
			enginePointer->d3d11DeviceContext->IAGetVertexBuffers(0, 1, oldVertexBuffer.GetAddressOf(), &oldStride, &oldOffset);
			enginePointer->d3d11DeviceContext->IAGetInputLayout(oldInputLayout.GetAddressOf());
			enginePointer->d3d11DeviceContext->VSGetShader(oldVertexShader.GetAddressOf(),NULL,NULL);
			enginePointer->d3d11DeviceContext->PSGetShader(oldPixelShader.GetAddressOf(),NULL,NULL);
			enginePointer->d3d11DeviceContext->PSGetSamplers(0, 1, oldTextureSamplerState.GetAddressOf());
			enginePointer->d3d11DeviceContext->IAGetPrimitiveTopology(&oldPrimitiveTopology);


			// Change what we need
			enginePointer->d3d11DeviceContext->IASetIndexBuffer(_indexBuffer11.Get(), DXGI_FORMAT_R32_UINT, 0);
			enginePointer->d3d11DeviceContext->IASetVertexBuffers(0, 1, _vertexBuffer11.GetAddressOf(), &stride, &offset);
			enginePointer->d3d11DeviceContext->IASetInputLayout(_vertexLayout11.Get());
			enginePointer->d3d11DeviceContext->VSSetShader(_pixelModeShader11.vertexShader11.Get(), NULL, NULL);
			enginePointer->d3d11DeviceContext->PSSetShader(_pixelModeShader11.pixelShader11.Get(), NULL, NULL);
			enginePointer->d3d11DeviceContext->PSSetSamplers(0, 1, _textureSamplerState11.GetAddressOf());
			enginePointer->d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			enginePointer->d3d11DeviceContext->PSSetShaderResources(0, 1, _textureShaderResourceView0_11.GetAddressOf());


			enginePointer->d3d11DeviceContext->DrawIndexed(6, 0, 0);

			// Restore old states
			enginePointer->d3d11DeviceContext->IASetIndexBuffer(oldIndexBuffer.Get(), oldIndexFormat, oldIndexOffset);
			enginePointer->d3d11DeviceContext->IASetVertexBuffers(0, 1, oldVertexBuffer.GetAddressOf(), &oldStride, &oldOffset);
			enginePointer->d3d11DeviceContext->IASetInputLayout(oldInputLayout.Get());
			enginePointer->d3d11DeviceContext->VSSetShader(oldVertexShader.Get(),NULL,NULL);
			enginePointer->d3d11DeviceContext->PSSetShader(oldPixelShader.Get(),NULL,NULL);
			enginePointer->d3d11DeviceContext->PSSetSamplers(0, 1, oldTextureSamplerState.GetAddressOf());
			if (oldPrimitiveTopology != D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
			{
				enginePointer->d3d11DeviceContext->IASetPrimitiveTopology(oldPrimitiveTopology);
			}
		}
#endif
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			// Draw the quad
			//enginePointer->commandList->SetPipelineState(_pipelineStateObject.Get());
			//enginePointer->commandList->SetGraphicsRootSignature(_rootSignature.Get());
			ID3D12DescriptorHeap* ppHeaps[] = { _frameBuffer.srvHeap.Get() };
			enginePointer->commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			//enginePointer->commandList->SetGraphicsRootDescriptorTable(0, _frameBuffer[_currentBuffer].srvHeap->GetGPUDescriptorHandleForHeapStart());

			//enginePointer->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
			//enginePointer->commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			//enginePointer->commandList->IASetIndexBuffer(&_indexBufferView);
			//enginePointer->commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
			enginePointer->commandList->ExecuteBundle(_renderBundle.Get());
		}
#endif
	}

	inline void PixelMode::Clear(const Color &color) noexcept
	{
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			std::fill_n(videoBuffer, _totalBufferSize, color.packedARGB);
			return;
		}
#endif	
		_threadPool.Queue(std::bind(std::fill_n<uint32_t*, uint32_t, uint32_t>, videoBuffers[_currentBuffer], _totalBufferSize, color.packedABGR));
		//std::fill_n(videoBuffer, _totalBufferSize, color.packedABGR); //1075
		_currentBuffer++;
		if (_currentBuffer > numbuffers - 1) _currentBuffer = 0;
		videoBuffer = videoBuffers[_currentBuffer];
		//ZeroMemory(videoBuffer, _totalBufferSize*4);// superfast
	}

	inline void PixelMode::Pixel(const int32_t x, const int32_t y, const game::Color& color) noexcept
	{
#if defined(_DEBUG)
		if (x < 0)
		{
			std::cout << "X < 0\n";
			return;
		}
		if (y < 0)
		{
			std::cout << "Y < 0\n";
			return;
		}
		if (x > _bufferSize.width - 1)
		{
			std::cout << "X > _bufferSize - 1\n";
			return;
		}
		if (y > _bufferSize.height - 1)
		{
			std::cout << "Y > _bufferSize - 1\n";
			return;
		}
#endif
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			videoBuffer[y * _bufferSize.width + x] = color.packedARGB;
			return;
		}
#endif
		videoBuffer[y * _bufferSize.width + x] = color.packedABGR;
	}

	inline void PixelMode::PixelClip(const int32_t x, const int32_t y, const game::Color& color) noexcept
	{
		if (x < 0 || y < 0) return;
		if (x > _bufferSize.width-1 || y > _bufferSize.height - 1) return;
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			videoBuffer[y * _bufferSize.width + x] = color.packedARGB;
			return;
		}
#endif
		videoBuffer[(y) * _bufferSize.width + x] = color.packedABGR;
	}

	inline void PixelMode::Line(int32_t x1, int32_t y1, const int32_t x2, const int32_t y2, const Color& color) noexcept
	{
		int32_t delta_x(x2 - x1);
		int32_t delta_y(y2 - y1);

		// if x1 == x2, then it does not matter what we set here
		const int8_t ix((delta_x > 0) - (delta_x < 0));
		delta_x = abs(delta_x) << 1;

		// if y1 == y2, then it does not matter what we set here
		const int8_t iy((delta_y > 0) - (delta_y < 0));
		delta_y = abs(delta_y) << 1;

		Pixel(x1, y1, color);

		if (delta_x >= delta_y)
		{
			// error may go below zero
			int32_t error(delta_y - (delta_x >> 1));

			while (x1 != x2)
			{
				if ((error >= 0) && (error || (ix > 0)))
				{
					error -= delta_x;
					y1 += iy;
				}
				// else do nothing

				error += delta_y;
				x1 += ix;

				Pixel(x1, y1, color);
			}
		}
		else
		{
			// error may go below zero
			int32_t error(delta_x - (delta_y >> 1));

			while (y1 != y2)
			{
				if ((error >= 0) && (error || (iy > 0)))
				{
					error -= delta_y;
					x1 += ix;
				}
				// else do nothing

				error += delta_x;
				y1 += iy;

				Pixel(x1, y1, color);
			}
		}
	}

	inline void PixelMode::LineClip(const int32_t x1, const int32_t y1, const int32_t x2, const int32_t y2, const Color& color) noexcept
	{
		//Liang - Barsky Algorithm

		struct _Clip
		{
			inline int clipTest(float_t p, float_t q, float_t* u1, float_t* u2) noexcept
			{
				float_t r(0.0f);

				if (p < 0.0) {
					r = q / p;
					if (r > *u2)
						return false;
					else
						if (r > *u1)
							*u1 = r;
				}
				else
					if (p > 0.0) {
						r = q / p;
						if (r < *u1)
							return false;
						else if (r < *u2)
							*u2 = r;
					}
					else
						if (q < 0.0)
							return false;

				return true;
			}
		};

		_Clip _clip;
		float_t u1(0.0f);
		float_t u2(1.0f);
		float_t dx((float_t)(x2 - x1));
		float_t dy(0);
		float_t x1f((float_t)x1);
		float_t x2f((float_t)x2);
		float_t y1f((float_t)y1);
		float_t y2f((float_t)y2);

		if (_clip.clipTest(-dx, x1f - 0, &u1, &u2))
			if (_clip.clipTest(dx, _bufferSize.width - 1 - x1f, &u1, &u2)) {
				dy = y2f - y1f;
				if (_clip.clipTest(-dy, y1f, &u1, &u2))
					if (_clip.clipTest(dy, _bufferSize.height - 1 - y1f, &u1, &u2)) {
						if (u2 < 1.0) {
							x2f = x1f + u2 * dx;
							y2f = y1f + u2 * dy;
						}
						if (u1 > 0.0) {
							x1f += u1 * dx;
							y1f += u1 * dy;
						}
						Line((int32_t)std::round(x1f), (int32_t)std::round(y1f), (int32_t)std::round(x2f), (int32_t)std::round(y2f), color);
					}
			}
	}

	inline void PixelMode::HLine(const int32_t x1, const int32_t x2, const int32_t y, const Color& color) noexcept
	{
		if (x1 == x2)
		{
			Pixel(x1, y, color);
			return;
		}
		int32_t start = (x1 < x2) ? x1 : x2;// std::min(x1, x2);
		int32_t end = (x1 > x2) ? x1 : x2;// std::max(x1, x2);

		for (int32_t startX = start; startX <= end; startX++)
		{
			Pixel(startX, y, color);
		}
	}

	inline void PixelMode::HLineClip(const int32_t x1, const int32_t x2, const int32_t y, const Color& color) noexcept
	{
		if (y < 0) return;
		if (y > _bufferSize.height - 1) return;

		int32_t start = (x1 < x2) ? x1 : x2;// std::min(x1, x2);
		int32_t end = (x1 > x2) ? x1 : x2;// std::max(x1, x2);

		if (start < 0) start = 0;
		if (end > _bufferSize.width - 1) end = _bufferSize.width - 1;
		
		if (start == end)
		{
			Pixel(start, y, color);
			return;
		}

		for (int32_t startX = start; startX <= end; startX++)
		{
			Pixel(startX, y, color);
		}
	}

	inline void PixelMode::VLine(const int32_t x, const int32_t y1, const int32_t y2, const Color& color) noexcept
	{
		if (y1 == y2)
		{
			Pixel(x, y1, color);
			return;
		}
		int32_t start = (y1 < y2) ? y1 : y2;
		int32_t end = (y1 > y2) ? y1 : y2;

		for (int32_t startY = start; startY <= end; startY++)
		{
			Pixel(x, startY, color);
		}
	}

	inline void PixelMode::VLineClip(const int32_t x, const int32_t y1, const int32_t y2, const Color& color) noexcept
	{
		if (x < 0) return;
		if (x > _bufferSize.width - 1) return;

		int32_t start = (y1 < y2) ? y1 : y2;
		int32_t end = (y1 > y2) ? y1 : y2;

		if (start < 0) start = 0;
		if (end > _bufferSize.height - 1) end = _bufferSize.height - 1;

		if (start == end)
		{
			Pixel(x, start, color);
			return;
		}

		for (int32_t startY = start; startY <= end; startY++)
		{
			Pixel(x, startY, color);
		}
	}

	inline void PixelMode::Circle(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept
	{
		if (radius < 0 || x < -radius || y < -radius || x - _bufferSize.width > radius || y - _bufferSize.height > radius)
			return;

		if (radius > 0)
		{
			int32_t x0 = 0;
			int32_t y0 = radius;
			int32_t d = 3 - 2 * radius;

			while (y0 >= x0) // only formulate 1/8 of circle
			{
				// Draw even octants
				Pixel(x + x0, y - y0, color);// Q6 - upper right right
				Pixel(x + y0, y + x0, color);// Q4 - lower lower right
				Pixel(x - x0, y + y0, color);// Q2 - lower left left
				Pixel(x - y0, y - x0, color);// Q0 - upper upper left
				if (x0 != 0 && x0 != y0)
				{
					Pixel(x + y0, y - x0, color);// Q7 - upper upper right
					Pixel(x + x0, y + y0, color);// Q5 - lower right right
					Pixel(x - y0, y + x0, color);// Q3 - lower lower left
					Pixel(x - x0, y - y0, color);// Q1 - upper left left
				}

				if (d < 0)
					d += 4 * x0++ + 6;
				else
					d += 4 * (x0++ - y0--) + 10;
			}
		}
		else
			Pixel(x, y, color);
	}

	inline void PixelMode::CircleClip(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept
	{
		if (radius < 0 || x < -radius || y < -radius || x - _bufferSize.width > radius || y - _bufferSize.height > radius)
			return;

		if (radius > 0)
		{
			int32_t x0(0);
			int32_t y0(radius);
			int32_t d(3 - 2 * radius);

			while (y0 >= x0) // only formulate 1/8 of circle
			{
				// Draw even octants
				PixelClip(x + x0, y - y0, color);// Q6 - upper right right
				PixelClip(x + y0, y + x0, color);// Q4 - lower lower right
				PixelClip(x - x0, y + y0, color);// Q2 - lower left left
				PixelClip(x - y0, y - x0, color);// Q0 - upper upper left
				if (x0 != 0 && x0 != y0)
				{
					PixelClip(x + y0, y - x0, color);// Q7 - upper upper right
					PixelClip(x + x0, y + y0, color);// Q5 - lower right right
					PixelClip(x - y0, y + x0, color);// Q3 - lower lower left
					PixelClip(x - x0, y - y0, color);// Q1 - upper left left
				}

				if (d < 0)
					d += 4 * x0++ + 6;
				else
					d += 4 * (x0++ - y0--) + 10;
			}
		}
		else
			PixelClip(x, y, color);
	}

	inline void PixelMode::CircleFilled(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept
	{
		if (radius < 0 || x < -radius || y < -radius || x - _bufferSize.width > radius || y - _bufferSize.height > radius)
			return;

		if (radius > 0)
		{
			int32_t x0 = 0;
			int32_t y0 = radius;
			int32_t d = 3 - 2 * radius;

			auto drawline = [&](int32_t sx, int32_t ex, int32_t y)
			{
				for (int32_t x = sx; x <= ex; x++)
					Pixel(x, y, color);
			};

			while (y0 >= x0)
			{
				drawline(x - y0, x + y0, y - x0);
				if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

				if (d < 0)
					d += 4 * x0++ + 6;
				else
				{
					if (x0 != y0)
					{
						drawline(x - x0, x + x0, y - y0);
						drawline(x - x0, x + x0, y + y0);
					}
					d += 4 * (x0++ - y0--) + 10;
				}
			}
		}
		else
			Pixel(x, y, color);
	}

	inline void PixelMode::CircleFilledClip(const int32_t x, const int32_t y, const int32_t radius, const Color& color) noexcept
	{
		if (radius < 0 || x < -radius || y < -radius || x - _bufferSize.width > radius || y - _bufferSize.height > radius)
			return;

		if (radius > 0)
		{
			int32_t x0 = 0;
			int32_t y0 = radius;
			int32_t d = 3 - 2 * radius;

			auto drawline = [&](int32_t sx, int32_t ex, int32_t y)
			{
				if (sx < 0) sx = 0;
				if (ex > _bufferSize.width - 1) ex = _bufferSize.width - 1;
				if (y < 0) return;// y = 0;
				if (y > _bufferSize.height - 1) return;// y = _bufferSize.height - 1;
				for (int x = sx; x <= ex; x++)
					Pixel(x, y, color);
			};

			while (y0 >= x0)
			{
				drawline(x - y0, x + y0, y - x0);
				if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

				if (d < 0)
					d += 4 * x0++ + 6;
				else
				{
					if (x0 != y0)
					{
						drawline(x - x0, x + x0, y - y0);
						drawline(x - x0, x + x0, y + y0);
					}
					d += 4 * (x0++ - y0--) + 10;
				}
			}
		}
		else
			Pixel(x, y, color);
	}

	inline void PixelMode::Rect(const Recti& rectangle, const Color& color) noexcept
	{
		// Top
		HLine(rectangle.left, rectangle.right, rectangle.top, color);
		// Bottom
		HLine(rectangle.left, rectangle.right, rectangle.bottom, color);
		// Left
		VLine(rectangle.left, rectangle.top, rectangle.bottom, color);
		// Right
		VLine(rectangle.right, rectangle.top, rectangle.bottom, color);
	}

	inline void PixelMode::RectClip(const Recti& rectangle, const Color& color) noexcept
	{
		// Top
		HLineClip(rectangle.left, rectangle.right, rectangle.top, color);
		// Bottom
		HLineClip(rectangle.left, rectangle.right, rectangle.bottom, color);
		// Left
		VLineClip(rectangle.left, rectangle.top, rectangle.bottom, color);
		// Right
		VLineClip(rectangle.right, rectangle.top, rectangle.bottom, color);
	}

	inline void PixelMode::RectFilled(const Recti& rectangle, const Color& color) noexcept
	{
		for (uint32_t y = rectangle.top; y <= (uint32_t)rectangle.bottom; y++)
		{
			HLine(rectangle.left, rectangle.right, y, color);
		}
	}

	inline void PixelMode::RectFilledClip(const Recti& rectangle, const Color& color) noexcept
	{
		for (uint32_t y = rectangle.top; y <= (uint32_t)rectangle.bottom; y++)
		{
			HLineClip(rectangle.left, rectangle.right, y, color);
		}
	}

	inline void PixelMode::HPillClip(const int32_t x, const int32_t y, const int32_t length, const int32_t radius, const game::Color& color) noexcept
	{
		int32_t calculatedLength = length - (radius * 2);

		if (radius < 0 || x < -length || y < -radius || x - _bufferSize.width > length || y - _bufferSize.height > radius)
			return;

		if (radius > 0)
		{
			int32_t x0 = 0;
			int32_t y0 = radius;
			int32_t d = 3 - 2 * radius;

			while (y0 >= x0)
			{
				HLineClip(x - y0, x + y0 + calculatedLength, y - x0, color);
				if (x0 > 0)
				{
					HLineClip(x - y0, x + y0 + calculatedLength, y + x0, color);
				}

				if (d < 0)
					d += 4 * x0++ + 6;
				else
				{
					if (x0 != y0)
					{
						HLineClip(x - x0, x + x0 + calculatedLength, y - y0, color);
						HLineClip(x - x0, x + x0 + calculatedLength, y + y0, color);
					}
					d += 4 * (x0++ - y0--) + 10;
				}
			}
		}
		else
			Pixel(x, y, color);
	}

	inline void PixelMode::VPillClip(const int32_t x, const int32_t y, const int32_t height, const int32_t radius, const game::Color& color) noexcept
	{
		int32_t calculatedHeight = y + (height - (radius * 2));

		if (radius < 0 || x < -radius || y < -height || x - _bufferSize.width > radius || y - _bufferSize.height > height)
			return;

		if (radius > 0)
		{
			int32_t x0 = 0;
			int32_t y0 = radius;
			int32_t d = 3 - 2 * radius;

			while (y0 >= x0)
			{
				// Top half
				HLineClip(x - y0, x + y0, y - x0, color);
				if (x0 > 0)
				{
					// Bottom half
					HLineClip(x - y0, x + y0, calculatedHeight + x0, color);
				}

				if (d < 0)
					d += 4 * x0++ + 6;
				else
				{
					if (x0 != y0)
					{
						// Top half
						HLineClip(x - x0, x + x0, y - y0, color);
						// Bottom half
						HLineClip(x - x0, x + x0, calculatedHeight + y0, color);
					}
					d += 4 * (x0++ - y0--) + 10;
				}
			}
		}
		else
			Pixel(x, y, color);

		for (int32_t width = -radius; width <= radius; width++)
		{
			VLineClip(x + width, y, calculatedHeight, color);
		}
	}

	inline void PixelMode::TextClip(const std::string& text, const int32_t x, const int32_t y, const game::Color& color, const uint32_t scale)
	{
		int32_t px = x;
		int32_t py = y;
		int32_t ox = 0;
		int32_t oy = 0;

		if (scale > 1)
		{
			for (uint8_t letter : text)
			{
				ox = (letter - 32) % 16;
				oy = (letter - 32) / 16;
				for (uint32_t i = 0; i < 8; i++)
				{
					for (uint32_t j = 0; j < 8; j++)
					{
						if (_fontROM[(j + oy * 8) * 128 + (i + ox * 8)] > 0)
						{
							for (uint32_t sy = 0; sy < scale; sy++)
							{
								for (uint32_t sx = 0; sx < scale; sx++)
								{
									PixelClip(px + (i * scale) + sx, py + (j * scale) + sy, color);
								}
							}
						}
					}
				}
				px += 8 * scale;
			}
			return;
		}
		else
		{
			for (uint8_t letter : text)
			{
				ox = (letter - 32) % 16;
				oy = (letter - 32) / 16;
				for (int32_t i = 0; i < 8; i++)
				{
					for (int32_t j = 0; j < 8; j++)
					{
						if (_fontROM[(j + oy * 8) * 128 + (i + ox * 8)] > 0)
							PixelClip(px + i, py + j, color);
					}
				}
				px += 8;
			}
		}

		//// then draw
//for (auto c : sText)
//{
//	if (c == '\n')
//	{
//		sx = 0; sy += 8 * scale;
//	}
//	else if (c == '\t')
//	{
//		sx += 8 * nTabSizeInSpaces * scale;
//	}
//	else
//	{
//		int32_t ox = (c - 32) % 16;
//		int32_t oy = (c - 32) / 16;

//		if (scale > 1)
//		{
//			for (uint32_t i = 0; i < 8; i++)
//				for (uint32_t j = 0; j < 8; j++)
//					if (fontRenderable.Sprite()->GetPixel(i + ox * 8, j + oy * 8).r > 0)
//						for (uint32_t is = 0; is < scale; is++)
//							for (uint32_t js = 0; js < scale; js++)
//								Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
//		}
	}

	inline void PixelMode::Text(const std::string& text, const int32_t x, const int32_t y, const game::Color& color, const uint32_t scale)
	{
		int32_t px = x;
		int32_t py = y;
		int32_t ox = 0;
		int32_t oy = 0;

		if (scale > 1)
		{
			for (uint8_t letter : text)
			{
				ox = (letter - 32) % 16;
				oy = (letter - 32) / 16;
				for (uint32_t i = 0; i < 8; i++)
				{
					for (uint32_t j = 0; j < 8; j++)
					{
						if (_fontROM[(j + oy * 8) * 128 + (i + ox * 8)] > 0)
						{
							for (uint32_t sy = 0; sy < scale; sy++)
							{
								for (uint32_t sx = 0; sx < scale; sx++)
								{
									Pixel(px + (i * scale) + sx, py + (j * scale) + sy, color);
								}
							}
						}
					}
				}
				px += 8 * scale;
			}
			return;
		}
		else
		{
			for (uint8_t letter : text)
			{
				ox = (letter - 32) % 16;
				oy = (letter - 32) / 16;
				for (int32_t i = 0; i < 8; i++)
				{
					for (int32_t j = 0; j < 8; j++)
					{
						if (_fontROM[(j + oy * 8) * 128 + (i + ox * 8)] > 0)
							Pixel(px + i, py + j, color);
					}
				}
				px += 8;
			}
		}
	}

	inline Pointi PixelMode::GetScaledMousePosition() const noexcept
	{
		Pointi scaledMouseCoords = enginePointer->geMouse.GetPosition();

		scaledMouseCoords.x = (int32_t)(scaledMouseCoords.x - _savedPositionOfScaledTexture.x);
		scaledMouseCoords.y = (int32_t)(scaledMouseCoords.y - _savedPositionOfScaledTexture.y);
		scaledMouseCoords.x = (int32_t)(scaledMouseCoords.x * _oneOverScale.x);
		scaledMouseCoords.y = (int32_t)(scaledMouseCoords.y * _oneOverScale.y);
		return scaledMouseCoords;
	}

	inline Pointi PixelMode::GetPixelFrameBufferSize() const noexcept
	{
		return _bufferSize;// { _bufferSize.width, _bufferSize.height };
	}
}

#undef PIXELMODEFVF

#endif