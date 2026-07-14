#pragma once

#include <cstdint>
#include <string>
#include "Game_Assert.h"
#include "GameMath.h"
#include "GameTexture2D.h"
#include "Game_SpriteSubSheet.h"

namespace game
{
	class SpriteSheet
	{
	public:
		SpriteSheet();
		SpriteSheet(const Texture2D& texture, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0);
		void Initialize(const Texture2D &texture, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0) noexcept;
		void Initialize(SpriteSubSheet& subSheet, const std::string& subName, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0);
		void Initialize(const Texture2D& texture, const Pointi& size, const game::Pointi &margin = { 0,0 }) noexcept;
		void Initialize(SpriteSubSheet& subSheet, const std::string& subName, const Pointi& size, const game::Pointi &margin = { 0,0 });
		Recti GetRectFromId(const int32_t id) const noexcept;
		uint32_t tileWidth;
		uint32_t tileHeight;
		//uint32_t GetTilesPerRow() const;
		//Pointi GetTextureOffset() const;
		//void SetTilesPerRow(const uint32_t tiles);
		//void SetTextureOffset(const Pointi offset);
	private:
		uint32_t _tilesPerRow;
		Pointi _textureOffset;
		Pointi _margins;
	};

	//void SpriteSheet::SetTilesPerRow(const uint32_t tiles)
	//{
	//	_tilesPerRow = tiles;
	//}

	//void SpriteSheet::SetTextureOffset(const Pointi offset)
	//{
	//	_textureOffset = offset;
	//}

	//uint32_t SpriteSheet::GetTilesPerRow() const
	//{
	//	return _tilesPerRow;
	//}

	//Pointi SpriteSheet::GetTextureOffset() const
	//{
	//	return _textureOffset;
	//}

	inline SpriteSheet::SpriteSheet()
	{
		tileWidth = 0;
		tileHeight = 0;
		_tilesPerRow = 0;
	}
	
	SpriteSheet::SpriteSheet(const Texture2D& texture, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY)
	{
		SpriteSheet();
		GAME_ASSERT(width);
		GAME_ASSERT(height);
		//if (!width || !height) return;
		tileWidth = width;
		tileHeight = height;
		_tilesPerRow = texture.width / (tileWidth + marginX);
		_margins.x = marginX;
		_margins.y = marginY;
	}

	void SpriteSheet::Initialize(SpriteSubSheet& subSheet, const std::string& subName, const Pointi& size, const game::Pointi &margin)
	{
		Initialize(subSheet, subName, size.width, size.height, margin.x, margin.y);
	}

	void SpriteSheet::Initialize(SpriteSubSheet& subSheet, const std::string& subName, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY)
	{
		GAME_ASSERT(!(subSheet.subTextureRegistry.find(subName) == subSheet.subTextureRegistry.end()));
		GAME_ASSERT(width);
		GAME_ASSERT(height);
		tileWidth = width;
		tileHeight = height;
		//int32_t textureWidth = subSheet.subTextureRegistry[subName].right;
		_tilesPerRow = subSheet.subTextureRegistry[subName].right / (tileWidth + marginX);
		_margins.x = marginX;
		_margins.y = marginY;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY) noexcept
	{
		GAME_ASSERT(width);
		GAME_ASSERT(height);
		tileWidth = width;
		tileHeight = height;
		_tilesPerRow = texture.width / (tileWidth+marginX);
		_margins.x = marginX;
		_margins.y = marginY;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const Pointi &size, const game::Pointi &margin) noexcept
	{
		Initialize(texture, size.width, size.height, margin.x, margin.y);
	}

	inline Recti SpriteSheet::GetRectFromId(const int32_t id) const noexcept
	{
		Recti rectangle;

		rectangle.top = (id / _tilesPerRow * (tileHeight + _margins.x)) + _textureOffset.y;
		rectangle.left = (id % _tilesPerRow * (tileWidth + _margins.y)) + _textureOffset.x;
		rectangle.bottom = rectangle.top + tileHeight;
		rectangle.right = rectangle.left + tileWidth;

		return rectangle;
	}
}