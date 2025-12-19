#pragma once
#if defined(GAME_OPENGL)
#include <GL/gl.h>
#endif
#if defined(GAME_DIRECTX9)
#include <d3d9.h>
#endif
#if defined(GAME_DIRECTX10)
#include <d3d10.h>
#endif

#include "GameEngine.h"
#include "GameErrors.h"
#include "GameMath.h"
#include "GameSpriteFont.h"
#include "GameTexture2D.h"
#include "GameTextShaders.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <iomanip>

namespace game
{
	extern GameError lastError;
	extern Engine* enginePointer;

	class SpriteBatch
	{
	public:
		SpriteBatch();
		~SpriteBatch();
		bool Initialize(const uint32_t maxSprites);
		// Sets up states for sprite rendering and saves old states
		void Begin();
		// Cleans up sprite rendering and restores previous states
		void End();
		// Render a complete batch of sprites
		void Render();
		// Will draw entire texture to location x,y
		void Draw(const Texture2D& texture, const uint32_t x, const uint32_t y, const Color color);
		void Draw(const Texture2D& texture, const Pointi& position, const Color color);
		// Will draw a specified rectangle portion of a texture to location x,y
		void Draw(const Texture2D& texture, const Recti& destination, const Recti& source, const Color& color);
		// Draws using floating point
		void Draw(const Texture2D& texture, const Rectf& destination, const Rectf& portion, const Color& color);

		void DrawString(const SpriteFont& font, const std::string& Str, const int x, const int y, const Color& color, const float_t scaleX = 1.0f, const float scaleY = -99999);
		void DrawStringWithTags(const SpriteFont& font, const std::string& Str, const int x, const int y, const Color& color, const float_t scaleX = 1.0f, const float scaleY = -99999);
		// How many sprites did it draw last frame
		uint32_t SpritesDrawnLastFrame() noexcept;
	private:
		uint32_t _maxSprites;
		uint32_t _numberOfSpritesUsed;
		Texture2D _currentTexture;
		uint32_t _spritesDrawnLastFrame;
		uint32_t _currentSpritesDrawn;
#if defined(GAME_OPENGL)
		struct _spriteVertexGL
		{
			float_t x, y, z, rhw;
			uint32_t color;
			float_t u, v;
		};
		_spriteVertexGL* _spriteVertices;
		uint32_t _oldTextureBound;
#endif
#if defined(GAME_DIRECTX9)
		struct _spriteVertex9
		{
			float_t x, y, z, rhw;
			uint32_t color;
			float_t u, v;
		};
		_spriteVertex9* _spriteVertices9;
		LPDIRECT3DVERTEXBUFFER9 _vertexBuffer9;
		LPDIRECT3DINDEXBUFFER9 _indexBuffer9;
		DWORD _savedFVF;
		DWORD _savedBlending;
		IDirect3DBaseTexture9* _savedTexture;
#endif
#if defined (GAME_DIRECTX10)
		struct _spriteVertex10
		{
			float_t x, y, z;
			float_t r, g, b, a;
			float_t u, v;
		};
		_spriteVertex10* _spriteVertices10;
		Shader _spriteBatchShader10;
		Microsoft::WRL::ComPtr<ID3D10Buffer> _vertexBuffer10;
		Microsoft::WRL::ComPtr<ID3D10InputLayout> _vertexLayout10;
		Microsoft::WRL::ComPtr<ID3D10SamplerState> _textureSamplerState10;
		Microsoft::WRL::ComPtr<ID3D10Buffer> _indexBuffer10;
		Microsoft::WRL::ComPtr<ID3D10BlendState> _spriteBatchBlendState10;
		Microsoft::WRL::ComPtr<ID3D10DepthStencilState> _depthStencilState10;


		// saves state of dx10 states we change to restore
		uint32_t _oldStride10;
		uint32_t _oldOffset10;
		uint32_t _oldStencilRef10;
		Microsoft::WRL::ComPtr<ID3D10Buffer> _oldVertexBuffer10;
		Microsoft::WRL::ComPtr<ID3D10Buffer> _oldIndexBuffer10;
		DXGI_FORMAT _oldIndexFormat10;
		uint32_t _oldIndexOffset10;
		Microsoft::WRL::ComPtr<ID3D10InputLayout> _oldInputLayout10;
		Microsoft::WRL::ComPtr<ID3D10VertexShader> _oldVertexShader10;
		Microsoft::WRL::ComPtr<ID3D10PixelShader> _oldPixelShader10;
		Microsoft::WRL::ComPtr<ID3D10SamplerState> _oldTextureSamplerState10;
		D3D10_PRIMITIVE_TOPOLOGY _oldPrimitiveTopology10;
		Microsoft::WRL::ComPtr<ID3D10BlendState> _oldBlendState10;
		Microsoft::WRL::ComPtr<ID3D10DepthStencilState> _oldDepthStencilState10;
		float_t _oldBlendFactor10[4];
		uint32_t _oldSampleMask10;
#endif
#if defined (GAME_DIRECTX11)
		struct _spriteVertex11
		{
			float_t x, y, z;
			float_t r, g, b, a;
			float_t u, v;
		};
		Shader _spriteBatchShader11;
		_spriteVertex11* _spriteVertices11;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer11;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> _vertexLayout11;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _textureSamplerState11;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _indexBuffer11;
		Microsoft::WRL::ComPtr<ID3D11BlendState> _spriteBatchBlendState11;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState11;

		// saves state of dx11 states we change to restore
		uint32_t _oldStride11;
		uint32_t _oldOffset11;
		uint32_t _oldStencilRef11;
		ID3D11Buffer* _oldVertexBuffer11;
		ID3D11Buffer* _oldIndexBuffer11;
		DXGI_FORMAT _oldIndexFormat11;
		uint32_t _oldIndexOffset11;
		ID3D11InputLayout* _oldInputLayout11;
		ID3D11VertexShader* _oldVertexShader11;
		ID3D11PixelShader* _oldPixelShader11;
		ID3D11SamplerState* _oldTextureSamplerState11;
		D3D11_PRIMITIVE_TOPOLOGY _oldPrimitiveTopology11;
		ID3D11BlendState* _oldBlendState11;
		ID3D11DepthStencilState* _oldDepthStencilState11;
		float_t _oldBlendFactor11[4];
		uint32_t _oldSampleMask11;
#endif
#if defined(GAME_DIRECTX12)
		struct _spriteVertex12
		{
			float_t x, y, z;
			float_t r, g, b, a;
			float_t u, v;
		};
		_spriteVertex12* _spriteVertices12;
		Shader _spriteBatchShader12;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineStateObject;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
		Microsoft::WRL::ComPtr<ID3D12Resource> _indexBufferHeap;
		//Microsoft::WRL::ComPtr<ID3D12Resource> _indexBufferUploadHeap;
		D3D12_INDEX_BUFFER_VIEW _indexBufferView;
		Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBufferHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBufferUploadHeap;
		D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _bundleAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _renderBundle;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _textureHeap;
		uint32_t _currentSRVIndex;
		uint32_t _maxSRVIndex;
		uint32_t _descriptorSize;
		uint32_t _vertexOffset;
		uint32_t _spritesUsed;
#endif
	};

	inline uint32_t SpriteBatch::SpritesDrawnLastFrame() noexcept
	{
		return _spritesDrawnLastFrame;
	}

	inline SpriteBatch::SpriteBatch()
	{
		_maxSprites = 0;
		_spritesDrawnLastFrame = 0;
		_currentSpritesDrawn = 0;

#if defined(GAME_OPENGL)
		//if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			_spriteVertices = nullptr;
			_oldTextureBound = 0;
		}
#endif
#if defined(GAME_DIRECTX9)  
		//if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			_spriteVertices9 = nullptr;
			_indexBuffer9 = nullptr;
			_vertexBuffer9 = nullptr;
			_savedFVF = 0;
			_savedBlending = 0;
			_savedTexture = nullptr;
		}
#endif
#if defined (GAME_DIRECTX10)
		//if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			
			_spriteVertices10 = nullptr;
			_oldStencilRef10 = 0;
			_oldStride10 = 0;
			_oldOffset10 = 0;
			_oldIndexFormat10 = {};
			_oldIndexOffset10 = 0;
			_oldPrimitiveTopology10 = {};
			ZeroMemory(_oldBlendFactor10, 4 * sizeof(float_t));
			_oldSampleMask10 = 0;
		}
#endif
#if defined (GAME_DIRECTX11)
		_spriteVertices11 = nullptr;
		_oldStride11 = 0;
		_oldOffset11 = 0;
		_oldStencilRef11 = 0;
		_oldVertexBuffer11 = nullptr;
		_oldIndexBuffer11 = nullptr;
		_oldIndexFormat11 = {};
		_oldIndexOffset11 = 0;
		_oldInputLayout11 = nullptr;
		_oldVertexShader11 = nullptr;
		_oldPixelShader11 = nullptr;
		_oldTextureSamplerState11 = nullptr;
		_oldPrimitiveTopology11 = {};
		_oldBlendState11 = nullptr;
		ZeroMemory(_oldBlendFactor11, 4 * sizeof(float_t));
		_oldDepthStencilState11 = nullptr;
		_oldSampleMask11 = 0;
#endif
#if defined(GAME_DIRECTX12)
		_vertexBufferView = {};
		_indexBufferView = {};
		_spriteVertices12 = nullptr;
		_maxSRVIndex = 20;
		_currentSRVIndex = 0;
		_descriptorSize = 0;
		_vertexOffset = 0;
		_spritesUsed = 0;
#endif
		_numberOfSpritesUsed = 0;
	}

	inline SpriteBatch::~SpriteBatch()
	{

#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			if (_spriteVertices)
			{
				delete[] _spriteVertices;
				_spriteVertices = nullptr;
			}
		}
#endif
#if defined (GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			SAFE_RELEASE(_vertexBuffer9);
			SAFE_RELEASE(_indexBuffer9);
			if (_spriteVertices9)
			{
				delete[] _spriteVertices9;
				_spriteVertices9 = nullptr;
			}
		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			if (_spriteVertices10)
			{
				delete[] _spriteVertices10;
				_spriteVertices10 = nullptr;
			}
			enginePointer->geUnLoadShader(_spriteBatchShader10);

		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			if (_spriteVertices11)
			{
				delete[] _spriteVertices11;
				_spriteVertices11 = nullptr;
			}
			enginePointer->geUnLoadShader(_spriteBatchShader11);
		}
#endif
#if defined (GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			if (_spriteVertices12)
			{
				delete[] _spriteVertices12;
				_spriteVertices12 = nullptr;
			}
			enginePointer->geUnLoadShader(_spriteBatchShader12);
		}
#endif
	}

	inline bool SpriteBatch::Initialize(const uint32_t maxSprites = 2000)
	{
		// Save max sprites
		_maxSprites = maxSprites;
		// OpenGL implementation of vertices
#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			_spriteVertices = new _spriteVertexGL[(uint64_t)(_maxSprites) * 6];
			for (uint32_t vertex = 0; vertex < _maxSprites * 6; vertex++)
			{
				_spriteVertices[vertex].x = 0.0f;
				_spriteVertices[vertex].y = 0.0f;
				_spriteVertices[vertex].z = 0.0f;
				_spriteVertices[vertex].rhw = 1.0f;
				_spriteVertices[vertex].u = 0.0f;
				_spriteVertices[vertex].v = 0.0f;
				_spriteVertices[vertex].color = Colors::White.packedABGR;
			}
		}
#endif

#if defined (GAME_DIRECTX9)  
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			_spriteVertices9 = new _spriteVertex9[(uint64_t)(_maxSprites) * 4];
			for (uint32_t vertex = 0; vertex < _maxSprites * 4; vertex++)
			{
				_spriteVertices9[vertex].x = 0.0f;
				_spriteVertices9[vertex].y = 0.0f;
				_spriteVertices9[vertex].z = 0.0f;
				_spriteVertices9[vertex].rhw = 1.0f;
				_spriteVertices9[vertex].u = 0.0f;
				_spriteVertices9[vertex].v = 0.0f;
				_spriteVertices9[vertex].color = Colors::White.packedARGB;
			}
		}
#endif

		// DX10 impementation of vertices
#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			_spriteVertices10 = new _spriteVertex10[(size_t)_maxSprites * 4];
			for (uint32_t vertex = 0; vertex < _maxSprites * 4; vertex++)
			{
				_spriteVertices10[vertex].x = 0.0f;
				_spriteVertices10[vertex].y = 0.0f;
				_spriteVertices10[vertex].z = 0.0f;
				_spriteVertices10[vertex].u = 0.0f;
				_spriteVertices10[vertex].v = 0.0f;
				_spriteVertices10[vertex].r = Colors::White.rf;
				_spriteVertices10[vertex].g = Colors::White.gf;
				_spriteVertices10[vertex].b = Colors::White.bf;
				_spriteVertices10[vertex].a = Colors::White.af;
			}
		}
#endif

		// DX11 impementation of vertices
#if defined(GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			_spriteVertices11 = new _spriteVertex11[(size_t)_maxSprites * 4];
			for (uint32_t vertex = 0; vertex < _maxSprites * 4; vertex++)
			{
				_spriteVertices11[vertex].x = 0.0f;
				_spriteVertices11[vertex].y = 0.0f;
				_spriteVertices11[vertex].z = 0.0f;
				_spriteVertices11[vertex].u = 0.0f;
				_spriteVertices11[vertex].v = 0.0f;
				_spriteVertices11[vertex].r = Colors::White.rf;
				_spriteVertices11[vertex].g = Colors::White.gf;
				_spriteVertices11[vertex].b = Colors::White.bf;
				_spriteVertices11[vertex].a = Colors::White.af;
			}
		}
#endif
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			_spriteVertices12 = new _spriteVertex12[(size_t)_maxSprites * 4];
			for (uint32_t vertex = 0; vertex < _maxSprites * 4; vertex++)
			{
				_spriteVertices12[vertex].x = 0.0f;
				_spriteVertices12[vertex].y = 0.0f;
				_spriteVertices12[vertex].z = 0.0f;
				_spriteVertices12[vertex].u = 0.0f;
				_spriteVertices12[vertex].v = 0.0f;
				_spriteVertices12[vertex].r = Colors::White.rf;
				_spriteVertices12[vertex].g = Colors::White.gf;
				_spriteVertices12[vertex].b = Colors::White.bf;
				_spriteVertices12[vertex].a = Colors::White.af;
	}
		}
#endif

		// Initialization of API methods used
#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
		}
#endif
#if defined (GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			uint32_t* indexMap = nullptr;
			std::vector<uint32_t> indices;

			//std::cout << "Vertex Buffer Size = " << _maxSprites * (uint32_t)6 * (uint32_t)sizeof(_spriteVertex) / 1024 << "kB.\n";
			if (enginePointer->d3d9Device->CreateVertexBuffer(_maxSprites * (uint32_t)4 * (uint32_t)sizeof(_spriteVertex9), 0, (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1), D3DPOOL_MANAGED, &_vertexBuffer9, NULL) != D3D_OK)
			{
				lastError = { GameErrors::GameDirectX9Specific, "Could not create vertex buffer for SpriteBatch." };
				return false;
			}

			if (enginePointer->d3d9Device->CreateIndexBuffer(_maxSprites * (uint32_t)6 * (uint32_t)sizeof(uint32_t), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &_indexBuffer9, NULL) != D3D_OK)
			{
				lastError = { GameErrors::GameDirectX9Specific, "Could not create index buffer for SpriteBatch." };
				return false;
			}

			// Fill index buffer
			for (uint32_t index = 0; index < _maxSprites; index++)
			{
				indices.emplace_back(0 + (index * 4));  // 4 indices per quad, not 6 like I had
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(2 + (index * 4));
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(3 + (index * 4));
				indices.emplace_back(2 + (index * 4));
			}

			// Copy data to gpu
			_indexBuffer9->Lock(0, 0, (void**)&indexMap, 0);
			memcpy(indexMap, indices.data(), sizeof(uint32_t) * 6 * _maxSprites);
			_indexBuffer9->Unlock();

		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			D3D10_BUFFER_DESC vertexBufferDescription = { 0 };
			D3D10_BUFFER_DESC indexBufferDescription = { 0 };
			D3D10_SUBRESOURCE_DATA vertexInitialData = { 0 };
			D3D10_SUBRESOURCE_DATA indexInitialData = { 0 };
			std::vector<uint32_t> indices;

			D3D10_INPUT_ELEMENT_DESC inputLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
			};
			D3D10_SAMPLER_DESC samplerDesc = { };

			// Load shaders for spriteBatch
			//if (!enginePointer->geLoadShader("Content/VertexShader.hlsl", "Content/PixelShader.hlsl", _spriteBatchShader10))
			if (!enginePointer->geLoadTextShader(gameTextShaders, "mainVS", "mainPS", _spriteBatchShader10))
			{
				return false;
			}

			// Create the vertex buffer
			vertexBufferDescription.ByteWidth = _maxSprites * (uint32_t)4 * (uint32_t)sizeof(_spriteVertex10);
			vertexBufferDescription.Usage = D3D10_USAGE_DYNAMIC;
			vertexBufferDescription.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			vertexBufferDescription.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			vertexBufferDescription.MiscFlags = 0;
			if (FAILED(enginePointer->d3d10Device->CreateBuffer(&vertexBufferDescription, NULL, _vertexBuffer10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific, "Could not create vertex buffer for SpriteBatch." };
				return false;
			}

			// Create index buffer
			//  0, 1, 2, 1, 3, 2
			for (uint32_t index = 0; index < _maxSprites; index++)
			{
				indices.emplace_back(0 + (index * 4));  // 4 indices per quad, not 6 like I had
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(2 + (index * 4));
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(3 + (index * 4));
				indices.emplace_back(2 + (index * 4));
			}
			indexBufferDescription.Usage = D3D10_USAGE_IMMUTABLE;
			indexBufferDescription.ByteWidth = sizeof(DWORD) * 6 * _maxSprites;
			indexBufferDescription.BindFlags = D3D10_BIND_INDEX_BUFFER;
			indexBufferDescription.CPUAccessFlags = 0;
			indexBufferDescription.MiscFlags = 0;
			indexInitialData.pSysMem = indices.data();
			if (FAILED(enginePointer->d3d10Device->CreateBuffer(&indexBufferDescription, &indexInitialData, _indexBuffer10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create index buffer for SpriteBatch." };
				enginePointer->geUnLoadShader(_spriteBatchShader10);
				return false;
			}


			// Create input layout for shaders
			if (FAILED(enginePointer->d3d10Device->CreateInputLayout(inputLayout, 3, _spriteBatchShader10.compiledVertexShader10->GetBufferPointer(), _spriteBatchShader10.compiledVertexShader10->GetBufferSize(), _vertexLayout10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific, "Could not create input layout for SpriteBatch." };
				return false;
			}

			// Create texture sampler 
			samplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT; //D3D10_FILTER_ANISOTROPIC
			samplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.ComparisonFunc = D3D10_COMPARISON_ALWAYS;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D10_FLOAT32_MAX;
			if (FAILED(enginePointer->d3d10Device->CreateSamplerState(&samplerDesc, _textureSamplerState10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create sampler state for SpriteBatch." };
				return false;
			}

			// Create blend state
			D3D10_BLEND_DESC blendStateDesc = { 0 };
			blendStateDesc.AlphaToCoverageEnable = FALSE;
			blendStateDesc.BlendEnable[0] = TRUE;
			blendStateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
			blendStateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
			blendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
			blendStateDesc.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
			blendStateDesc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
			blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
			blendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
			if (FAILED(enginePointer->d3d10Device->CreateBlendState(&blendStateDesc, _spriteBatchBlendState10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific, "Could not create blend state for SpriteBatch." };
				return false;
			}

			// Create depth state
			D3D10_DEPTH_STENCIL_DESC dsDesc = { 0 };
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK::D3D10_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D10_COMPARISON_FUNC::D3D10_COMPARISON_LESS_EQUAL;

			// Create depth stencil state
			if (FAILED(enginePointer->d3d10Device->CreateDepthStencilState(&dsDesc, _depthStencilState10.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific, "Could not create Depth Stencil State. " };
				return false;
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
			std::vector<uint32_t> indices;

			D3D11_INPUT_ELEMENT_DESC inputLayout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			D3D11_SAMPLER_DESC samplerDesc = { };

			// Load shaders for spriteBatch
			//if (!enginePointer->geLoadShader("Content/VertexShader.hlsl", "Content/PixelShader.hlsl", _spriteBatchShader11))
			if (!enginePointer->geLoadTextShader(gameTextShaders, "mainVS", "mainPS", _spriteBatchShader11))
			{
				return false;
			}

			// Create the vertex buffer
			vertexBufferDescription.ByteWidth = _maxSprites * (uint32_t)4 * (uint32_t)sizeof(_spriteVertex11);
			vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDescription.MiscFlags = 0;
			if (FAILED(enginePointer->d3d11Device->CreateBuffer(&vertexBufferDescription, NULL, _vertexBuffer11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX11Specific, "Could not create vertex buffer for SpriteBatch." };
				return false;
			}

			// Create index buffer
			//  0, 1, 2, 1, 3, 2
			for (uint32_t index = 0; index < _maxSprites; index++)
			{
				indices.emplace_back(0 + (index * 4));  // 4 indices per quad, not 6 like I had
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(2 + (index * 4));
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(3 + (index * 4));
				indices.emplace_back(2 + (index * 4));
			}
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.ByteWidth = sizeof(DWORD) * 6 * _maxSprites;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDescription.CPUAccessFlags = 0;
			indexBufferDescription.MiscFlags = 0;
			indexInitialData.pSysMem = indices.data();
			if (FAILED(enginePointer->d3d11Device->CreateBuffer(&indexBufferDescription, &indexInitialData, _indexBuffer11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX11Specific,"Could not create index buffer for SpriteBatch." };
				enginePointer->geUnLoadShader(_spriteBatchShader11);
				return false;
			}


			// Create input layout for shaders
			if (FAILED(enginePointer->d3d11Device->CreateInputLayout(inputLayout, 3, _spriteBatchShader11.compiledVertexShader11->GetBufferPointer(), _spriteBatchShader11.compiledVertexShader11->GetBufferSize(), _vertexLayout11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX11Specific, "Could not create input layout for SpriteBatch." };
				return false;
			}

			// Create texture sampler 
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D10_FILTER_ANISOTROPIC
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			if (FAILED(enginePointer->d3d11Device->CreateSamplerState(&samplerDesc, _textureSamplerState11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific,"Could not create sampler state for SpriteBatch." };
				return false;
			}

			// Create blend state
			D3D11_BLEND_DESC blendStateDesc = { 0 };
			blendStateDesc.AlphaToCoverageEnable = FALSE;
			blendStateDesc.IndependentBlendEnable = FALSE;
			blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			if (FAILED(enginePointer->d3d11Device->CreateBlendState(&blendStateDesc, _spriteBatchBlendState11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX11Specific, "Could not create blend state for SpriteBatch." };
				return false;
			}

			// Create depth state
			D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

			// Create depth stencil state
			if (FAILED(enginePointer->d3d11Device->CreateDepthStencilState(&dsDesc, _depthStencilState11.GetAddressOf())))
			{
				lastError = { GameErrors::GameDirectX10Specific, "Could not create Depth Stencil State. " };
				return false;
			}
		}
#endif 
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			// Root signature description
			HRESULT hr = {};
			// Create the root signature.

			CD3DX12_DESCRIPTOR_RANGE1 ranges[1] = {};
			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, _maxSRVIndex, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

			CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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
				lastError = { GameErrors::GameDirectX12Specific, "Could not serialize root signature in SpriteBatch." };
				AppendHR12(hr);
				return false;
			}

			// create the root signature
			hr = enginePointer->d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific, "Could not create root signature in SpriteBatch." };
				AppendHR12(hr);
				return false;
			}
			_rootSignature->SetName(L"SpriteBatch Root Signature");


			// Load shaders for sprite mode
			//if (!enginePointer->geLoadShader("Content/VertexShader.hlsl", "Content/PixelShader.hlsl", _spriteBatchShader12))
			if (!enginePointer->geLoadTextShader(gameTextShaders, "mainVS", "mainPS", _spriteBatchShader12))
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
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(_spriteBatchShader12.compiledVertexShader12.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(_spriteBatchShader12.compiledPixelShader12.Get());
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
				lastError = { GameErrors::GameDirectX12Specific,"Could not create pipline state for SpriteBatch." };
				AppendHR12(hr);
				return false;
			}
			_pipelineStateObject->SetName(L"SpriteBatch PSO");

			// Create vertex buffer
			uint32_t vBufferSize = _maxSprites * sizeof(float_t) * (uint32_t)sizeof(_spriteVertex12);

			// Create vertex buffer heap
			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_CUSTOM);
			D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_vertexBufferHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create vertex buffer heap for SpriteBatch." };
				AppendHR12(hr);
				return false;
			}
			_vertexBufferHeap->SetName(L"SpriteBatch Vertex Buffer Heap");

			// Create vertex buffer upload heap
			heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			resDesc = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexBufferUploadHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create vertex buffer upload heap for SpriteBatch." };
				AppendHR12(hr);
				return false;
			}
			_vertexBufferUploadHeap->SetName(L"SpriteBatch Vertex Buffer Upload Heap");


			// Index buffer
			//DWORD iList[] = {
			//	0, 1, 2,
			//	1, 3, 2
			//};
			//uint32_t iBufferSize = sizeof(iList);
			std::vector<uint32_t> indices;
			for (uint32_t index = 0; index < _maxSprites; index++)
			{
				indices.emplace_back(0 + (index * 4));  // 4 indices per quad, not 6 like I had
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(2 + (index * 4));
				indices.emplace_back(1 + (index * 4));
				indices.emplace_back(3 + (index * 4));
				indices.emplace_back(2 + (index * 4));
			}
			uint32_t iBufferSize = (uint32_t)indices.size() * sizeof(uint32_t);

			// Create index buffer heap
			heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			resDesc = CD3DX12_RESOURCE_DESC::Buffer(iBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_indexBufferHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create index buffer upload heap for SpriteBatch." };
				AppendHR12(hr);
				return false;
			}
			_indexBufferHeap->SetName(L"SpriteBatch Index Buffer Heap");

			// Create a temporary upload heap to upload index buffer
			Microsoft::WRL::ComPtr<ID3D12Resource> _indexBufferUploadHeap;
			heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			resDesc = CD3DX12_RESOURCE_DESC::Buffer(iBufferSize);
			hr = enginePointer->d3d12Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_indexBufferUploadHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create vertex buffer upload heap for SpriteBatch." };
				AppendHR12(hr);
				return false;
			}
			_indexBufferUploadHeap->SetName(L"SpriteBatch Index Buffer Upload Heap");

			RendererDX12* temp = enginePointer->geGetRenderer();
			// resets the command list -----------------------------
			{
				// Reset to start recording
				if (FAILED(enginePointer->commandList->Reset(temp->_commandAllocator[temp->_frameIndex].Get(), NULL)))
				{
					std::cout << "command list reset failed\n";
				}
			}
			// end of command list reset ---------------------------------------

			// Fill out resource data with vertex data
			D3D12_SUBRESOURCE_DATA vertexData = {};
			vertexData.pData = reinterpret_cast<uint8_t*>(_spriteVertices12);
			vertexData.RowPitch = sizeof(_spriteVertex12);
			vertexData.SlicePitch = 0;

			// Create a command to copy vertex buffer
			UpdateSubresources(enginePointer->commandList.Get(), _vertexBufferHeap.Get(), _vertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

			// Transition the vertex buffer data from copy destination state to vertex buffer state
			CD3DX12_RESOURCE_BARRIER resBar = CD3DX12_RESOURCE_BARRIER::Transition(_vertexBufferHeap.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			enginePointer->commandList->ResourceBarrier(1, &resBar);

			// Fill out resource data with index buffer
			D3D12_SUBRESOURCE_DATA indexData = {};
			indexData.pData = reinterpret_cast<uint8_t*>(indices.data());
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

				// Set fence for this section
				temp->_fenceValue[temp->_frameIndex]++;
				hr = enginePointer->commandQueue->Signal(temp->_fence[temp->_frameIndex].Get(), temp->_fenceValue[temp->_frameIndex]);
				if (FAILED(hr))
				{
					lastError = { GameErrors::GameDirectX12Specific,"SpriteBatch signal failed." };
					AppendHR12(hr);
					return false;
				}
			}

			// Create a vertex buffer view for the quad
			_vertexBufferView.BufferLocation = _vertexBufferHeap->GetGPUVirtualAddress();
			_vertexBufferView.StrideInBytes = sizeof(_spriteVertex12);
			_vertexBufferView.SizeInBytes = vBufferSize;

			// Create a index buffer view for the quad
			_indexBufferView.BufferLocation = _indexBufferHeap->GetGPUVirtualAddress();
			_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			_indexBufferView.SizeInBytes = iBufferSize;

			// Create a texture heap for use
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = _maxSRVIndex;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			hr = enginePointer->d3d12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_textureHeap));
			if (FAILED(hr))
			{
				lastError = { GameErrors::GameDirectX12Specific,"Could not create texture srv heap for SpriteBatch." };
				AppendHR12(hr);
				return false;
			}

			_descriptorSize = enginePointer->d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			enginePointer->geGetRenderer()->flushGPU();
		}
#endif
		return true;
	}

	inline void SpriteBatch::Begin()
	{
		_spritesDrawnLastFrame = _currentSpritesDrawn;
		_currentSpritesDrawn = 0;
		// needs to save and retore ALL changed states
#if defined (GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			// Save current state
			enginePointer->d3d9Device->GetFVF(&_savedFVF);
			enginePointer->d3d9Device->GetTexture(0, &_savedTexture);
			enginePointer->d3d9Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &_savedBlending);

			// Disable multisampling if enabled
			if (enginePointer->_attributes.MultiSamples > 1)
			{
				enginePointer->d3d9Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
			}

			enginePointer->d3d9Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			enginePointer->d3d9Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			enginePointer->d3d9Device->SetFVF((D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1));
			enginePointer->d3d9Device->SetStreamSource(0, _vertexBuffer9, 0, sizeof(_spriteVertex9));
			enginePointer->d3d9Device->SetIndices(_indexBuffer9);
		}
#endif
#if defined(GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			uint32_t stride = sizeof(_spriteVertex10);
			uint32_t offset = 0;
			float sampleMask[] = { 1.0f, 1.0f, 1.0f, 1.0f };

			// need to save blend state check Render

			// Save everything we modify
			enginePointer->d3d10Device->IAGetIndexBuffer(_oldIndexBuffer10.GetAddressOf(), &_oldIndexFormat10, &_oldIndexOffset10);
			enginePointer->d3d10Device->IAGetVertexBuffers(0, 1, _oldVertexBuffer10.GetAddressOf(), &_oldStride10, &_oldOffset10);
			enginePointer->d3d10Device->IAGetInputLayout(_oldInputLayout10.GetAddressOf());
			enginePointer->d3d10Device->VSGetShader(_oldVertexShader10.GetAddressOf());
			enginePointer->d3d10Device->PSGetShader(_oldPixelShader10.GetAddressOf());
			enginePointer->d3d10Device->PSGetSamplers(0, 1, _oldTextureSamplerState10.GetAddressOf());
			enginePointer->d3d10Device->IAGetPrimitiveTopology(&_oldPrimitiveTopology10);
			enginePointer->d3d10Device->OMGetBlendState(_oldBlendState10.GetAddressOf(), _oldBlendFactor10, &_oldSampleMask10);
			enginePointer->d3d10Device->OMGetDepthStencilState(_oldDepthStencilState10.GetAddressOf(), &_oldStencilRef10);

			// Change what we need
			enginePointer->d3d10Device->IASetIndexBuffer(_indexBuffer10.Get(), DXGI_FORMAT_R32_UINT, 0);
			enginePointer->d3d10Device->IASetVertexBuffers(0, 1, _vertexBuffer10.GetAddressOf(), &stride, &offset);
			enginePointer->d3d10Device->IASetInputLayout(_vertexLayout10.Get());
			enginePointer->d3d10Device->VSSetShader(_spriteBatchShader10.vertexShader10.Get());
			enginePointer->d3d10Device->PSSetShader(_spriteBatchShader10.pixelShader10.Get());
			enginePointer->d3d10Device->PSSetSamplers(0, 1, _textureSamplerState10.GetAddressOf());
			enginePointer->d3d10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			enginePointer->d3d10Device->OMSetBlendState(_spriteBatchBlendState10.Get(), sampleMask, 0xffffffff);
			enginePointer->d3d10Device->OMSetDepthStencilState(_depthStencilState10.Get(), 1);
			

			// Reset current texture
			_currentTexture.name = "";
			// Disable multisampling
			// not now
		}
#endif
#if defined(GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			uint32_t stride = sizeof(_spriteVertex11);
			uint32_t offset = 0;
			float sampleMask[] = { 1.0f, 1.0f, 1.0f, 1.0f };

			// need to save blend state check Render

			// Save everything we modify
			enginePointer->d3d11DeviceContext->IAGetIndexBuffer(&_oldIndexBuffer11, &_oldIndexFormat11, &_oldIndexOffset11);
			enginePointer->d3d11DeviceContext->IAGetVertexBuffers(0, 1, &_oldVertexBuffer11, &_oldStride11, &_oldOffset11);
			enginePointer->d3d11DeviceContext->IAGetInputLayout(&_oldInputLayout11);
			enginePointer->d3d11DeviceContext->VSGetShader(&_oldVertexShader11, NULL, NULL);
			enginePointer->d3d11DeviceContext->PSGetShader(&_oldPixelShader11, NULL, NULL);
			enginePointer->d3d11DeviceContext->PSGetSamplers(0, 1, &_oldTextureSamplerState11);
			enginePointer->d3d11DeviceContext->IAGetPrimitiveTopology(&_oldPrimitiveTopology11);
			enginePointer->d3d11DeviceContext->OMGetBlendState(&_oldBlendState11, _oldBlendFactor11, &_oldSampleMask11);
			enginePointer->d3d11DeviceContext->OMGetDepthStencilState(&_oldDepthStencilState11, &_oldStencilRef11);

			// Change what we need
			enginePointer->d3d11DeviceContext->IASetIndexBuffer(_indexBuffer11.Get(), DXGI_FORMAT_R32_UINT, 0);
			enginePointer->d3d11DeviceContext->IASetVertexBuffers(0, 1, _vertexBuffer11.GetAddressOf(), &stride, &offset);
			enginePointer->d3d11DeviceContext->IASetInputLayout(_vertexLayout11.Get());
			enginePointer->d3d11DeviceContext->VSSetShader(_spriteBatchShader11.vertexShader11.Get(), NULL, NULL);
			enginePointer->d3d11DeviceContext->PSSetShader(_spriteBatchShader11.pixelShader11.Get(), NULL, NULL);
			enginePointer->d3d11DeviceContext->PSSetSamplers(0, 1, _textureSamplerState11.GetAddressOf());
			enginePointer->d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			enginePointer->d3d11DeviceContext->OMSetBlendState(_spriteBatchBlendState11.Get(), sampleMask, 0xffffffff);
			enginePointer->d3d11DeviceContext->OMSetDepthStencilState(_depthStencilState11.Get(), 1);

			// Reset current texture
			_currentTexture.name = "";
			// Disable multisampling
			// not now
		}
#endif
#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			enginePointer->commandList->SetPipelineState(_pipelineStateObject.Get());
			enginePointer->commandList->SetGraphicsRootSignature(_rootSignature.Get());
			enginePointer->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_vertexBufferView.BufferLocation = _vertexBufferHeap->GetGPUVirtualAddress();
			enginePointer->commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			enginePointer->commandList->IASetIndexBuffer(&_indexBufferView);
			ID3D12DescriptorHeap* ppHeaps[] = { _textureHeap.Get() };
			enginePointer->commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
			_spritesUsed = 0;
		}
#endif


#if defined (GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{

			// Save current texture
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&_oldTextureBound);
			glPushAttrib(GL_TEXTURE_2D);
			glEnable(GL_TEXTURE_2D);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPushAttrib(GL_BLEND);
			glEnable(GL_BLEND);	
			
			// and texture bound to it
		}
#endif

	}

	inline void SpriteBatch::End()
	{
		if (_numberOfSpritesUsed > 0)
		{
			Render();
		}

#if defined (GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{

			// Restore previous state
			enginePointer->d3d9Device->SetFVF(_savedFVF);
			enginePointer->d3d9Device->SetTexture(0, _savedTexture);
			enginePointer->d3d9Device->SetRenderState(D3DRS_ALPHABLENDENABLE, _savedBlending);
			if (_savedTexture)
			{
				// This may be a bug
				_savedTexture->Release();
				_savedTexture = nullptr;
			}

			// Renable multisampling if it was enabled
			if (enginePointer->_attributes.MultiSamples > 1)
			{
				enginePointer->d3d9Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
			}
		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			// restore everything
			enginePointer->d3d10Device->OMSetDepthStencilState(_oldDepthStencilState10.Get(), _oldStencilRef10);
			enginePointer->d3d10Device->IASetIndexBuffer(_oldIndexBuffer10.Get(), _oldIndexFormat10, _oldIndexOffset10);
			enginePointer->d3d10Device->IASetVertexBuffers(0, 1, _oldVertexBuffer10.GetAddressOf(), &_oldStride10, &_oldOffset10);
			enginePointer->d3d10Device->IASetInputLayout(_oldInputLayout10.Get());
			enginePointer->d3d10Device->VSSetShader(_oldVertexShader10.Get());
			enginePointer->d3d10Device->PSSetShader(_oldPixelShader10.Get());
			enginePointer->d3d10Device->PSSetSamplers(0, 1, _oldTextureSamplerState10.GetAddressOf());
			if (_oldPrimitiveTopology10 != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
			{
				enginePointer->d3d10Device->IASetPrimitiveTopology(_oldPrimitiveTopology10);
			}
			enginePointer->d3d10Device->OMSetBlendState(_oldBlendState10.Get(), _oldBlendFactor10, _oldSampleMask10);
		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			// restore everything
			enginePointer->d3d11DeviceContext->IASetIndexBuffer(_oldIndexBuffer11, _oldIndexFormat11, _oldIndexOffset11);
			enginePointer->d3d11DeviceContext->IASetVertexBuffers(0, 1, &_oldVertexBuffer11, &_oldStride11, &_oldOffset11);
			enginePointer->d3d11DeviceContext->IASetInputLayout(_oldInputLayout11);
			enginePointer->d3d11DeviceContext->VSSetShader(_oldVertexShader11, NULL, NULL);
			enginePointer->d3d11DeviceContext->PSSetShader(_oldPixelShader11, NULL, NULL);
			enginePointer->d3d11DeviceContext->PSSetSamplers(0, 1, &_oldTextureSamplerState11);
			if (_oldPrimitiveTopology11 != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
			{
				enginePointer->d3d11DeviceContext->IASetPrimitiveTopology(_oldPrimitiveTopology11);
			}
			enginePointer->d3d11DeviceContext->OMSetBlendState(_oldBlendState11, _oldBlendFactor11, _oldSampleMask11);
			enginePointer->d3d11DeviceContext->OMSetDepthStencilState(_oldDepthStencilState11, _oldStencilRef11);
		}
#endif
#if defined (GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			// restore saved this stuff
			if (_oldTextureBound)
			{
				glBindTexture(GL_TEXTURE_2D, _oldTextureBound);
			}
			glPopAttrib(); // GL_BLEND
			glPopAttrib(); // GL_TEXTURE_2D
		}
#endif
#if defined (GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			_numberOfSpritesUsed = 0;
			_vertexOffset = 0;
			_spritesUsed = 0;
		}
#endif
	}

	inline void SpriteBatch::Render()
	{
		if (_numberOfSpritesUsed == 0)
		{
			return;
		}
		_currentSpritesDrawn += _numberOfSpritesUsed;

#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			void* lockedDest = nullptr;
			uint32_t sizeToCopy = sizeof(_spriteVertex9) * 4 * _numberOfSpritesUsed;

			// Send sprite vertices to gpu
			_vertexBuffer9->Lock(0, sizeToCopy, (void**)&lockedDest, 0);
			memcpy(lockedDest, &_spriteVertices9[0], sizeToCopy);
			_vertexBuffer9->Unlock();

			// Draw the sprites
			enginePointer->d3d9Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _numberOfSpritesUsed * 6, 0, _numberOfSpritesUsed * 2);
		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			void* lockedDest = nullptr;

			// Send vertices to card
			_vertexBuffer10->Map(D3D10_MAP_WRITE_DISCARD, 0, &lockedDest);
			memcpy(lockedDest, &_spriteVertices10[0], sizeof(_spriteVertex10) * 4 * _numberOfSpritesUsed);
			_vertexBuffer10->Unmap();

			// Draw the sprites
			enginePointer->d3d10Device->DrawIndexed(_numberOfSpritesUsed * 6, 0, 0);
		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			D3D11_MAPPED_SUBRESOURCE lockedDest = {};

			// Send vertices to card
			enginePointer->d3d11DeviceContext->Map(_vertexBuffer11.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &lockedDest);
			memcpy(lockedDest.pData, &_spriteVertices11[0], sizeof(_spriteVertex11) * _numberOfSpritesUsed * 4);
			enginePointer->d3d11DeviceContext->Unmap(_vertexBuffer11.Get(), 0);

			// Draw the sprites
			enginePointer->d3d11DeviceContext->DrawIndexed(_numberOfSpritesUsed * 6, 0, 0);
		}
#endif

#if defined(GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{

			// Copy vertex data to gpu

			uint8_t* lockedDest = nullptr;
			HRESULT hr = _vertexBufferHeap.Get()->Map(0, NULL, (void**)&lockedDest);
			if (FAILED(hr))
			{
				AppendHR12(hr);
				std::cout << lastError.lastErrorString << "\n";
			}
			uint32_t usedBufferOffset = _spritesUsed * (uint32_t)4 * (uint32_t)sizeof(_spriteVertex12);
			uint32_t newSpriteBytes = (_numberOfSpritesUsed - _spritesUsed) * (uint32_t)4 * (uint32_t)sizeof(_spriteVertex12);
			memcpy(lockedDest + usedBufferOffset, reinterpret_cast<BYTE*>(_spriteVertices12) + usedBufferOffset, newSpriteBytes);
			_vertexBufferHeap.Get()->Unmap(0, 0);


			// Allocate space from the GPU-visible descriptor heap.
			CD3DX12_CPU_DESCRIPTOR_HANDLE toHandle(_textureHeap->GetCPUDescriptorHandleForHeapStart());
			toHandle.Offset(_currentSRVIndex, _descriptorSize);

			// Copy from the various CPU-visible descriptors you have for it.
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(_currentTexture.srvHeap->GetCPUDescriptorHandleForHeapStart());
			enginePointer->d3d12Device->CopyDescriptorsSimple(1, toHandle, cpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			// Draw the sprites
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(_textureHeap->GetGPUDescriptorHandleForHeapStart());
			gpuHandle.Offset(_currentSRVIndex, _descriptorSize);
			enginePointer->commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);

			//enginePointer->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
			//enginePointer->commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			//enginePointer->commandList->IASetIndexBuffer(&_indexBufferView);
			
			//_vertexBufferView.BufferLocation = _vertexBufferHeap->GetGPUVirtualAddress() + (_spritesUsed) *(uint32_t)4 * (uint32_t)sizeof(_spriteVertex12);
			enginePointer->commandList->DrawIndexedInstanced((_numberOfSpritesUsed - _spritesUsed) * 6, 1, _spritesUsed * 6, 0, 0);


			return;
		}
#endif

#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			float_t r, g, b, a = 0.0f;
			Vector2i windowSize = enginePointer->geGetWindowSize();

			glBindTexture(GL_TEXTURE_2D, _currentTexture.bind);

			_spriteVertexGL* access = &_spriteVertices[0];

			float pixelOffsetFixX = 1.0f + (1.0f / windowSize.width);
			float pixelOffsetFixY = 1.0f + (1.0f / windowSize.height);

			glBegin(GL_QUADS);


			for (uint32_t i = 0; i < _numberOfSpritesUsed; i++)
			{
				r = (access->color & 0xff) / 255.0f;
				g = (access->color >> 8 & 0xff) / 255.0f;
				b = (access->color >> 16 & 0xff) / 255.0f;
				a = (access->color >> 24 & 0xff) / 255.0f;

				// Bottom left
				glTexCoord2f(access->u, access->v);
				glColor4f(r, g, b, a);
				access->x = (access->x * 2.0f / (float_t)windowSize.width) - pixelOffsetFixX;
				access->y = (access->y * 2.0f / (float_t)windowSize.height) - pixelOffsetFixY;
				access->y *= -1.0f;
				glVertex2f(access->x, access->y);
				access++;

				// Bottom right
				glTexCoord2f(access->u, access->v);
				glColor4f(r, g, b, a);
				access->x = (access->x * 2.0f / (float_t)windowSize.width) - pixelOffsetFixX;
				access->y = (access->y * 2.0f / (float_t)windowSize.height) - pixelOffsetFixY;
				access->y *= -1.0f;
				glVertex2f(access->x, access->y);
				access++;

				// Top right
				glTexCoord2f(access->u, access->v);
				glColor4f(r, g, b, a);
				access->x = (access->x * 2.0f / (float_t)windowSize.width) - pixelOffsetFixX;
				access->y = (access->y * 2.0f / (float_t)windowSize.height) - pixelOffsetFixY;
				access->y *= -1.0f;
				glVertex2f(access->x, access->y);
				access++;

				// Bottom left
				glTexCoord2f(access->u, access->v);
				glColor4f(r, g, b, a);
				access->x = (access->x * 2.0f / (float_t)windowSize.width) - pixelOffsetFixX;
				access->y = (access->y * 2.0f / (float_t)windowSize.height) - pixelOffsetFixY;
				access->y *= -1.0f;
				glVertex2f(access->x, access->y);
				access++;
			}


			glEnd();
		}
#endif
		_numberOfSpritesUsed = 0;
	}

	inline void SpriteBatch::Draw(const Texture2D& texture, const Pointi& position, const Color color)
	{
		Draw(texture, position.x, position.y, color);
	}

	inline void SpriteBatch::Draw(const Texture2D& texture, const uint32_t x, const uint32_t y, const Color color)
	{
		if (_numberOfSpritesUsed + 1 >= _maxSprites)
		{
			Render();
		}

#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			if (texture.bind != _currentTexture.bind)
			{
				Render();
				_currentTexture = texture;
			}
			_spriteVertexGL* access = &_spriteVertices[_numberOfSpritesUsed * 4];

			// bl
			access->x = (float_t)x;
			access->y = (float_t)y + (float_t)texture.height;
			access->u = 0.0f;
			access->v = 1.0f;
			access->color = color.packedABGR;
			access++;

			// br
			access->x = (float_t)x + (float_t)texture.width;
			access->y = (float_t)y + (float_t)texture.height;
			access->u = 1.0f;
			access->v = 1.0f;
			access->color = color.packedABGR;
			access++;

			// tr
			access->x = (float_t)x + (float_t)texture.width;
			access->y = (float_t)y;
			access->u = 1.0f;
			access->v = 0.0f;
			access->color = color.packedABGR;
			access++;

			// tl
			access->x = (float_t)x;
			access->y = (float_t)y;
			access->u = 0.0f;
			access->v = 0.0f;
			access->color = color.packedABGR;
			//access++;

		}
#endif

#if defined(GAME_DIRECTX9)

		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			if (texture.textureInterface9 != _currentTexture.textureInterface9)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d9Device->SetTexture(0, _currentTexture.textureInterface9);
			}
			_spriteVertex9* access = &_spriteVertices9[_numberOfSpritesUsed * 4];
			// Top left
			access->x = (float_t)x;
			access->y = (float_t)y;
			access->u = 0.0f;
			access->v = 0.0f;
			access->color = color.packedARGB;
			access++;

			// Top right
			access->x = (float_t)x + (float_t)texture.width;
			access->y = (float_t)y;
			access->u = 1.0f;
			access->v = 0.0f;
			access->color = color.packedARGB;
			access++;

			// Bottom left
			access->x = (float_t)x;
			access->y = (float_t)y + (float_t)texture.height;
			access->u = 0.0f;
			access->v = 1.0f;
			access->color = color.packedARGB;
			access++;

			// Bottom right
			access->x = (float_t)x + (float_t)texture.width;
			access->y = (float_t)y + (float_t)texture.height;
			access->u = 1.0f;
			access->v = 1.0f;
			access->color = color.packedARGB;
			access++;
		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			_spriteVertex10* access = nullptr;
			Vector2i windowSize;
			Rectf scaledPos;

			// If texture changed, render and change SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d10Device->PSSetShaderResources(0, 1, texture.textureSRV10.GetAddressOf());
			}

			access = &_spriteVertices10[_numberOfSpritesUsed * 4];
			windowSize = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPos.left = ((float_t)x * 2.0f / (float_t)windowSize.width) - 1.0f;
			scaledPos.top = 1.0f - ((float_t)y * 2.0f / (float_t)windowSize.height);// -1.0f;
			scaledPos.right = (((float_t)x + (float_t)texture.width) * 2.0f / (float)windowSize.width) - 1.0f;
			scaledPos.bottom = 1.0f - (((float_t)y + (float_t)texture.height) * 2.0f / (float)windowSize.height);// -1.0f;


			// Fill vertices

			// Top left
			access->x = scaledPos.left;
			access->y = scaledPos.top;
			access->u = 0.0f;
			access->v = 0.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPos.right;
			access->y = scaledPos.top;
			access->u = 1.0f;
			access->v = 0.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPos.left;
			access->y = scaledPos.bottom;
			access->u = 0.0f;
			access->v = 1.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPos.right;
			access->y = scaledPos.bottom;
			access->u = 1.0f;
			access->v = 1.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			_spriteVertex11* access = nullptr;
			Vector2i windowSize;
			Rectf scaledPos;

			// If texture changed, render and change SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d11DeviceContext->PSSetShaderResources(0, 1, texture.textureSRV11.GetAddressOf());
			}

			access = &_spriteVertices11[_numberOfSpritesUsed * 4];
			windowSize = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPos.left = ((float_t)x * 2.0f / (float_t)windowSize.width) - 1.0f;
			scaledPos.top = 1.0f - ((float_t)y * 2.0f / (float_t)windowSize.height);// -1.0f;
			scaledPos.right = (((float_t)x + (float_t)texture.width) * 2.0f / (float)windowSize.width) - 1.0f;
			scaledPos.bottom = 1.0f - (((float_t)y + (float_t)texture.height) * 2.0f / (float)windowSize.height);// -1.0f;


			// Fill vertices

			// Top left
			access->x = scaledPos.left;
			access->y = scaledPos.top;
			access->u = 0.0f;
			access->v = 0.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPos.right;
			access->y = scaledPos.top;
			access->u = 1.0f;
			access->v = 0.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPos.left;
			access->y = scaledPos.bottom;
			access->u = 0.0f;
			access->v = 1.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPos.right;
			access->y = scaledPos.bottom;
			access->u = 1.0f;
			access->v = 1.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif
#if defined (GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			_spriteVertex12* access = nullptr;
			Vector2i windowSize;
			Rectf scaledPos;

			// If texture changed, render and change SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				_currentSRVIndex++;
				if (_currentSRVIndex >= _maxSRVIndex)
				{
					_currentSRVIndex = 0;
				}
				_spritesUsed = _numberOfSpritesUsed;
			}
			access = &_spriteVertices12[_numberOfSpritesUsed * 4];
			windowSize = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPos.left = ((float_t)x * 2.0f / (float_t)windowSize.width) - 1.0f;
			scaledPos.top = 1.0f - ((float_t)y * 2.0f / (float_t)windowSize.height);
			scaledPos.right = (((float_t)x + (float_t)texture.width) * 2.0f / (float)windowSize.width) - 1.0f;
			scaledPos.bottom = 1.0f - (((float_t)y + (float_t)texture.height) * 2.0f / (float)windowSize.height);


			// Fill vertices

			// Top left
			access->x = scaledPos.left;
			access->y = scaledPos.top;
			access->u = 0.0f;
			access->v = 0.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPos.right;
			access->y = scaledPos.top;
			access->u = 1.0f;
			access->v = 0.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPos.left;
			access->y = scaledPos.bottom;
			access->u = 0.0f;
			access->v = 1.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPos.right;
			access->y = scaledPos.bottom;
			access->u = 1.0f;
			access->v = 1.0f;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif

		_numberOfSpritesUsed++;
	}

	inline void SpriteBatch::Draw(const Texture2D& texture, const Recti& destination, const Recti& portion, const Color& color)
	{
		if (_numberOfSpritesUsed + 1 >= _maxSprites)
		{
			Render();
		}
#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			Vector2i windowSize = enginePointer->geGetWindowSize();
			if (texture.bind != _currentTexture.bind)
			{
				Render();
				_currentTexture = texture;
			}
			_spriteVertexGL* access = &_spriteVertices[_numberOfSpritesUsed * 4];

			// Bottom left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedABGR;
			access++;

			// Bottom right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedABGR;
			access++;

			// Top right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedABGR;
			access++;

			// Top left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedABGR;
			//access++;
		}
#endif
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			if (texture.textureInterface9 != _currentTexture.textureInterface9)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d9Device->SetTexture(0, _currentTexture.textureInterface9);
			}

			_spriteVertex9* access = &_spriteVertices9[_numberOfSpritesUsed * 4];
			// Top left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;

			// Top right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;// 1.0f;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;

			// Bottom left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;

			// Bottom right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;
		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			_spriteVertex10* access = nullptr;
			Vector2i window;
			Rectf scaledPosition;
			Rectf scaledUV;

			// If texture changed, render and change texture/SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d10Device->PSSetShaderResources(0, 1, _currentTexture.textureSRV10.GetAddressOf());
			}

			access = &_spriteVertices10[_numberOfSpritesUsed * 4];
			window = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPosition.left = ((float_t)(destination.left) * 2.0f / (float_t)window.width) - 1.0f;
			scaledPosition.top = 1.0f - ((float_t)(destination.top) * 2.0f / (float_t)window.height);// -1.0f;
			scaledPosition.right = (((float_t)destination.right) * 2.0f / (float)window.width) - 1.0f;
			scaledPosition.bottom = 1.0f - (((float_t)destination.bottom) * 2.0f / (float)window.height);// -1.0f;
			// Homogenise UV coords to 0.0f - 1.0f
			scaledUV.left = (float_t)portion.left * texture.oneOverWidth;
			scaledUV.top = (float_t)portion.top * texture.oneOverHeight;
			scaledUV.right = (float_t)portion.right * texture.oneOverWidth;
			scaledUV.bottom = (float_t)portion.bottom * texture.oneOverHeight;

			// Fill vertices

			// Top left
			access->x = scaledPosition.left;
			access->y = scaledPosition.top;
			access->u = scaledUV.left;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Top right
			access->x = scaledPosition.right;
			access->y = scaledPosition.top;
			access->u = scaledUV.right;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPosition.left;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.left;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPosition.right;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.right;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			_spriteVertex11* access = nullptr;
			Vector2i window;
			Rectf scaledPosition;
			Rectf scaledUV;

			// If texture changed, render and change texture/SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d11DeviceContext->PSSetShaderResources(0, 1, texture.textureSRV11.GetAddressOf());
			}

			access = &_spriteVertices11[_numberOfSpritesUsed * 4];
			window = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPosition.left = ((float_t)destination.left * 2.0f / (float_t)window.width) - 1.0f;
			scaledPosition.top = 1.0f - ((float_t)destination.top * 2.0f / (float_t)window.height);// -1.0f;
			scaledPosition.right = (((float_t)destination.right) * 2.0f / (float)window.width) - 1.0f;
			scaledPosition.bottom = 1.0f - (((float_t)destination.bottom) * 2.0f / (float)window.height);// -1.0f;
			// Homogenise UV coords to 0.0f - 1.0f
			scaledUV.left = (float_t)portion.left * texture.oneOverWidth;
			scaledUV.top = (float_t)portion.top * texture.oneOverHeight;
			scaledUV.right = (float_t)portion.right * texture.oneOverWidth;
			scaledUV.bottom = (float_t)portion.bottom * texture.oneOverHeight;

			// Fill vertices

			// Top left
			access->x = scaledPosition.left;
			access->y = scaledPosition.top;
			access->u = scaledUV.left;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPosition.right;
			access->y = scaledPosition.top;
			access->u = scaledUV.right;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPosition.left;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.left;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPosition.right;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.right;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif
#if defined (GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			_spriteVertex12* access = nullptr;
			Vector2i windowSize;
			Rectf scaledPosition;
			Rectf scaledUV;

			// If texture changed, render and change SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				_currentSRVIndex++;
				if (_currentSRVIndex >= _maxSRVIndex)
				{
					_currentSRVIndex = 0;
				}
				_spritesUsed = _numberOfSpritesUsed;
			}

			access = &_spriteVertices12[_numberOfSpritesUsed * 4];
			windowSize = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPosition.left = ((float_t)destination.left * 2.0f / (float_t)windowSize.width) - 1.0f;
			scaledPosition.top = 1.0f - ((float_t)destination.top * 2.0f / (float_t)windowSize.height);
			scaledPosition.right = (((float_t)destination.right) * 2.0f / (float)windowSize.width) - 1.0f;
			scaledPosition.bottom = 1.0f - (((float_t)destination.bottom) * 2.0f / (float)windowSize.height);
			// Homogenise UV coords to 0.0f - 1.0f
			scaledUV.left = (float_t)portion.left * texture.oneOverWidth;
			scaledUV.top = (float_t)portion.top * texture.oneOverHeight;
			scaledUV.right = (float_t)portion.right * texture.oneOverWidth;
			scaledUV.bottom = (float_t)portion.bottom * texture.oneOverHeight;

			// Fill vertices

			// Top left
			access->x = scaledPosition.left;
			access->y = scaledPosition.top;
			access->u = scaledUV.left;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPosition.right;
			access->y = scaledPosition.top;
			access->u = scaledUV.right;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPosition.left;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.left;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPosition.right;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.right;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif

		_numberOfSpritesUsed++;
	}

	inline void SpriteBatch::Draw(const Texture2D& texture, const Rectf& destination, const Rectf& portion, const Color& color)
	{
		if (_numberOfSpritesUsed + 1 >= _maxSprites)
		{
			Render();
		}
#if defined(GAME_OPENGL)
		if (enginePointer->geIsUsing(GAME_OPENGL))
		{
			Vector2i windowSize = enginePointer->geGetWindowSize();
			if (texture.bind != _currentTexture.bind)
			{
				Render();
				_currentTexture = texture;
			}
			_spriteVertexGL* access = &_spriteVertices[_numberOfSpritesUsed * 4];

			// Bottom left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedABGR;
			access++;

			// Bottom right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedABGR;
			access++;

			// Top right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedABGR;
			access++;

			// Top left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedABGR;
			//access++;
		}
#endif
#if defined(GAME_DIRECTX9)
		if (enginePointer->geIsUsing(GAME_DIRECTX9))
		{
			if (texture.textureInterface9 != _currentTexture.textureInterface9)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d9Device->SetTexture(0, _currentTexture.textureInterface9);
			}

			_spriteVertex9* access = &_spriteVertices9[_numberOfSpritesUsed * 4];
			// Top left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;

			// Top right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.top - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;// 1.0f;
			access->v = (float_t)portion.top * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;

			// Bottom left
			access->x = (float_t)destination.left - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.left * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;

			// Bottom right
			access->x = (float_t)destination.right - texture.oneOverWidth;
			access->y = (float_t)destination.bottom - texture.oneOverHeight;
			access->u = (float_t)portion.right * texture.oneOverWidth;
			access->v = (float_t)portion.bottom * texture.oneOverHeight;
			access->color = color.packedARGB;
			access++;
		}
#endif
#if defined (GAME_DIRECTX10)
		if (enginePointer->geIsUsing(GAME_DIRECTX10))
		{
			_spriteVertex10* access = nullptr;
			Vector2i window;
			Rectf scaledPosition;
			Rectf scaledUV;

			// If texture changed, render and change texture/SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d10Device->PSSetShaderResources(0, 1, _currentTexture.textureSRV10.GetAddressOf());
			}

			access = &_spriteVertices10[_numberOfSpritesUsed * 4];
			window = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPosition.left = ((float_t)(destination.left) * 2.0f / (float_t)window.width) - 1.0f;
			scaledPosition.top = 1.0f - ((float_t)(destination.top) * 2.0f / (float_t)window.height);// -1.0f;
			scaledPosition.right = (((float_t)destination.right) * 2.0f / (float)window.width) - 1.0f;
			scaledPosition.bottom = 1.0f - (((float_t)destination.bottom) * 2.0f / (float)window.height);// -1.0f;
			// Homogenise UV coords to 0.0f - 1.0f
			scaledUV.left = (float_t)portion.left * texture.oneOverWidth;
			scaledUV.top = (float_t)portion.top * texture.oneOverHeight;
			scaledUV.right = (float_t)portion.right * texture.oneOverWidth;
			scaledUV.bottom = (float_t)portion.bottom * texture.oneOverHeight;

			// Fill vertices

			// Top left
			access->x = scaledPosition.left;
			access->y = scaledPosition.top;
			access->u = scaledUV.left;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Top right
			access->x = scaledPosition.right;
			access->y = scaledPosition.top;
			access->u = scaledUV.right;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPosition.left;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.left;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPosition.right;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.right;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
		}
#endif
#if defined (GAME_DIRECTX11)
		if (enginePointer->geIsUsing(GAME_DIRECTX11))
		{
			_spriteVertex11* access = nullptr;
			Vector2i window;
			Rectf scaledPosition;
			Rectf scaledUV;

			// If texture changed, render and change texture/SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				enginePointer->d3d11DeviceContext->PSSetShaderResources(0, 1, texture.textureSRV11.GetAddressOf());
			}

			access = &_spriteVertices11[_numberOfSpritesUsed * 4];
			window = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPosition.left = ((float_t)destination.left * 2.0f / (float_t)window.width) - 1.0f;
			scaledPosition.top = 1.0f - ((float_t)destination.top * 2.0f / (float_t)window.height);// -1.0f;
			scaledPosition.right = (((float_t)destination.right) * 2.0f / (float)window.width) - 1.0f;
			scaledPosition.bottom = 1.0f - (((float_t)destination.bottom) * 2.0f / (float)window.height);// -1.0f;
			// Homogenise UV coords to 0.0f - 1.0f
			scaledUV.left = (float_t)portion.left * texture.oneOverWidth;
			scaledUV.top = (float_t)portion.top * texture.oneOverHeight;
			scaledUV.right = (float_t)portion.right * texture.oneOverWidth;
			scaledUV.bottom = (float_t)portion.bottom * texture.oneOverHeight;

			// Fill vertices

			// Top left
			access->x = scaledPosition.left;
			access->y = scaledPosition.top;
			access->u = scaledUV.left;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPosition.right;
			access->y = scaledPosition.top;
			access->u = scaledUV.right;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPosition.left;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.left;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPosition.right;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.right;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif
#if defined (GAME_DIRECTX12)
		if (enginePointer->geIsUsing(GAME_DIRECTX12))
		{
			_spriteVertex12* access = nullptr;
			Vector2i windowSize;
			Rectf scaledPosition;
			Rectf scaledUV;

			// If texture changed, render and change SRV
			if (texture.name != _currentTexture.name)
			{
				Render();
				_currentTexture = texture;
				_currentSRVIndex++;
				if (_currentSRVIndex >= _maxSRVIndex)
				{
					_currentSRVIndex = 0;
				}
				_spritesUsed = _numberOfSpritesUsed;
			}

			access = &_spriteVertices12[_numberOfSpritesUsed * 4];
			windowSize = enginePointer->geGetWindowSize();
			// Homogenise coordinates to -1.0f to 1.0f
			scaledPosition.left = ((float_t)destination.left * 2.0f / (float_t)windowSize.width) - 1.0f;
			scaledPosition.top = 1.0f - ((float_t)destination.top * 2.0f / (float_t)windowSize.height);
			scaledPosition.right = (((float_t)destination.right) * 2.0f / (float)windowSize.width) - 1.0f;
			scaledPosition.bottom = 1.0f - (((float_t)destination.bottom) * 2.0f / (float)windowSize.height);
			// Homogenise UV coords to 0.0f - 1.0f
			scaledUV.left = (float_t)portion.left * texture.oneOverWidth;
			scaledUV.top = (float_t)portion.top * texture.oneOverHeight;
			scaledUV.right = (float_t)portion.right * texture.oneOverWidth;
			scaledUV.bottom = (float_t)portion.bottom * texture.oneOverHeight;

			// Fill vertices

			// Top left
			access->x = scaledPosition.left;
			access->y = scaledPosition.top;
			access->u = scaledUV.left;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;

			access++;

			// Top right
			access->x = scaledPosition.right;
			access->y = scaledPosition.top;
			access->u = scaledUV.right;
			access->v = scaledUV.top;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom left
			access->x = scaledPosition.left;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.left;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;

			// Bottom right
			access->x = scaledPosition.right;
			access->y = scaledPosition.bottom;
			access->u = scaledUV.right;
			access->v = scaledUV.bottom;
			access->r = color.rf;
			access->g = color.gf;
			access->b = color.bf;
			access->a = color.af;
			access++;
		}
#endif

		_numberOfSpritesUsed++;
	}

	void SpriteBatch::DrawString(const SpriteFont& font, const std::string& Str, const int x, const int y, const Color& color, const float_t scaleX, const float scaleY)
	{
		const float_t _scaleY = scaleY == -99999 ? scaleX : scaleY;
		const float_t _scaleX = scaleX;
		float_t currentX = (float_t)x;
		float_t currentY = (float_t)y;
		//uint32_t widthOfLetter = 0;
		//uint32_t heightOfLetter = 0;
		Rectf source, destination;
		//int16_t letter;

		const uint64_t size = Str.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			const uint8_t letter = Str[i];
			const uint32_t widthOfLetter = font._characterSet.letters[letter].width;
			const uint32_t heightOfLetter = font._characterSet.letters[letter].height;

			source.left = font._characterSet.letters[letter].x;
			source.top = font._characterSet.letters[letter].y;
			source.right = source.left + widthOfLetter;
			source.bottom = source.top + heightOfLetter;

			destination.left = currentX + (font._characterSet.letters[letter].xOffset * _scaleX);
			destination.top = currentY + (font._characterSet.letters[letter].yOffset * _scaleY);
			destination.right = destination.left + (widthOfLetter * _scaleX);
			destination.bottom = destination.top + (heightOfLetter * _scaleY);

			// make a draw for Rectf
			Draw(font.Texture(), destination, source, color);

			currentX += (font._characterSet.letters[letter].xAdvance * _scaleX);
		}
	}


	// Font tag helpers
	namespace
	{
		// Structure to hold a text segment and its color
		class TextSegment
		{
		public:
			std::string text;
			Color color; // 0xRRGGBB
		};

		// Helper: Convert hex string to integer
		static Color hexToRGBA(const std::string& hex, const Color defaultColor)
		{
			if (hex.size() != 8) return defaultColor; // default white
			uint32_t rgba = 0;
			rgba = std::stoul(hex, nullptr, 16);
			//std::stringstream ss;
			//ss << std::hex << hex;
			//ss >> rgba;
			Color ret;
			uint32_t r = (rgba >> 24) & 0xFF;
			uint32_t g = (rgba >> 16) & 0xFF;
			uint32_t b = (rgba >> 8) & 0xFF;
			uint32_t a = (rgba >> 0) & 0xFF;
			ret.Set(r, g, b, a);
			return  ret;
		}

		// Parser function
		static std::vector<TextSegment> parseColoredString(const std::string& input, const Color& defaultColor = Colors::White)
		{
			std::vector<TextSegment> segments;
			uint64_t pos = 0;
			Color currentColor = defaultColor;
			const uint64_t size = input.size();
			std::string coloredText;
			std::string colorCode;
			uint64_t colorCodeStart = 0;
			uint64_t tagEnd = 0;
			uint64_t closeTag = 0;
			uint64_t tagStart = 0;

			while (pos < size)
			{
				tagStart = input.find("<color=#", pos);

				if (tagStart == std::string::npos)
				{
					// No more tags, push remaining text
					segments.push_back({ input.substr(pos), currentColor });
					break;
				}

				// Push text before tag
				if (tagStart > pos)
				{
					segments.push_back({ input.substr(pos, tagStart - pos), currentColor });
				}

				// Parse color code
				colorCodeStart = tagStart + 8; // skip "<color=#"
				colorCode = input.substr(colorCodeStart, 8);
				currentColor = hexToRGBA(colorCode, defaultColor);

				// Find closing '>'
				tagEnd = input.find('>', colorCodeStart + 8);
				if (tagEnd == std::string::npos)
				{
					break; // malformed tag
				}

				// Find closing </color>
				closeTag = input.find("</color>", tagEnd);
				if (closeTag == std::string::npos)
				{
					break; // malformed tag
				}

				// Extract colored text
				coloredText = input.substr(tagEnd + 1, closeTag - (tagEnd + 1));
				segments.push_back({ coloredText, currentColor });

				// Reset color after closing tag
				currentColor = defaultColor;

				// Move position after closing tag
				pos = closeTag + 8; // length of "</color>"
			}

			return segments;
		}
	}

	void SpriteBatch::DrawStringWithTags(const SpriteFont& font, const std::string& Str, const int x, const int y, const Color& color, const float_t scaleX, const float scaleY)
	{
		//if (scaleY == -99999) scaleY = scaleX;
		const float_t _scaleY = scaleY == -99999 ? scaleX : scaleY;
		const float_t _scaleX = scaleX;
		float_t currentX = (float)x;
		float_t currentY = (float)y;
		//uint32_t widthOfLetter = 0;
		//uint32_t heightOfLetter = 0;
		Rectf source, destination;
		//int16_t letter = 0;

		auto segments = parseColoredString(Str, color);
		for (auto &s:segments)
		{
			const uint64_t size = s.text.size();
			for (uint64_t i = 0; i < size; ++i)
			{
				const uint8_t letter = s.text[i];
				const uint32_t widthOfLetter = font._characterSet.letters[letter].width;
				const uint32_t heightOfLetter = font._characterSet.letters[letter].height;

				source.left = font._characterSet.letters[letter].x;
				source.top = font._characterSet.letters[letter].y;
				source.right = source.left + widthOfLetter;
				source.bottom = source.top + heightOfLetter;

				destination.left = currentX+(font._characterSet.letters[letter].xOffset * _scaleX);
				destination.top = currentY+(font._characterSet.letters[letter].yOffset * _scaleY);
				destination.right = destination.left + (widthOfLetter * _scaleX);
				destination.bottom = destination.top + (heightOfLetter * _scaleY);

				Draw(font.Texture(), destination, source, s.color);

				currentX += (font._characterSet.letters[letter].xAdvance * _scaleX);

			}
		}
	}
}
