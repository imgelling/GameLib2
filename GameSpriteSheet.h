#pragma once

#include <cstdint>
#include "GameMath.h"
#include "GameTexture2D.h"
#include "Game_SpriteSubSheet.h"

namespace game
{
	class SpriteSheet
	{
	public:
		SpriteSheet();
		SpriteSheet(Texture2D& texure, const int width, const int height);
		void Initialize(Texture2D &texure, const int width, const int height) noexcept;
		void Initialize(game::SpriteSubSheet& subSheet, const std::string& subName, const int32_t width, const int32_t height);
		void Initialize(Texture2D &texure, const Pointi &size) noexcept;
		void SetTexture(Texture2D& texture) { _texture = &texture; };
		Recti GetRectFromId(int id) noexcept;
	private:
		uint32_t _tileWidth;
		uint32_t _tileHeight;
		uint32_t _tilesPerRow;
		Texture2D* _texture;
		game::Pointi _textureOffset;
	};

	inline SpriteSheet::SpriteSheet()
	{
		_tileWidth = 0;
		_tileHeight = 0;
		_tilesPerRow = 0;
		_texture = nullptr;
	}
	
	SpriteSheet::SpriteSheet(Texture2D& texture, const int width, const int height)
	{
		SpriteSheet();
		if (!width || !height) return;
		_texture = &texture;
		_tileWidth = width;
		_tileHeight = height;
		_tilesPerRow = _texture->width / _tileWidth;
	}

	void SpriteSheet::Initialize(game::SpriteSubSheet& subSheet, const std::string& subName, const int32_t width, const int32_t height)
	{
		auto it = subSheet.subTexture.find(subName);
		if (it == subSheet.subTexture.end())
			return;

		_texture = &subSheet.texture;
		_tileWidth = width;
		_tileHeight = height;
		int32_t textureWidth = subSheet.subTexture[subName].right - subSheet.subTexture[subName].left;
		int32_t textureHeight = subSheet.subTexture[subName].bottom - subSheet.subTexture[subName].top;
		_tilesPerRow = textureWidth / _tileWidth;
		//_textureOffset.y = subSheet.subTexture[subName].top;
		//_textureOffset.x = subSheet.subTexture[subName].left;
	}

	inline void SpriteSheet::Initialize(Texture2D &texture, const int width, const int height) noexcept
	{
		_texture = &texture;
		_tileWidth = width;
		_tileHeight = height;
		_tilesPerRow = _texture->width / _tileWidth;
	}

	inline void SpriteSheet::Initialize(Texture2D &texture, const Pointi &size) noexcept
	{
		Initialize(texture, size.width, size.height);
	}

	inline Recti SpriteSheet::GetRectFromId(int id) noexcept
	{
		Recti rectangle;
		if (!_texture)
		{
			return rectangle;
		}

		rectangle.left = (id % _tilesPerRow * _tileWidth) + _textureOffset.x;
		rectangle.top = (id / _tilesPerRow * _tileHeight) + _textureOffset.y;
		rectangle.right = rectangle.left + _tileWidth;
		rectangle.bottom = rectangle.top + _tileHeight;

		return rectangle;
	}
}